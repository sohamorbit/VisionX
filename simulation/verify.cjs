const fs = require('fs'), avr = require('avr8js');
const bytes = new Uint8Array(32768);
for (const line of fs.readFileSync(process.argv[2], 'utf8').trim().split(/\r?\n/)) {
  if (line.slice(7,9) !== '00') continue;
  const offset=parseInt(line.slice(3,7),16);
  Buffer.from(line.slice(9,-2),'hex').copy(bytes,offset);
}
const cpu=new avr.CPU(new Uint16Array(bytes.buffer));
const pd=new avr.AVRIOPort(cpu, avr.portDConfig), pb=new avr.AVRIOPort(cpu,avr.portBConfig);
for(const cfg of [avr.timer0Config,avr.timer1Config,avr.timer2Config]) new avr.AVRTimer(cpu,cfg);
// Acknowledge the display address and count SSD1306 data transactions.
const twi=new avr.AVRTWI(cpu,avr.twiConfig,16e6);
let bytesInTransaction=0, ramWrites=0;
twi.eventHandler={
  start(){bytesInTransaction=0;twi.completeStart();},
  stop(){twi.completeStop();},
  connectToSlave(addr,write){twi.completeConnect(addr===0x3c && write);},
  writeByte(value){if(bytesInTransaction++===0 && value===0x40)ramWrites++;twi.completeWrite(true);},
  readByte(){twi.completeRead(255);}
};
const uart=new avr.AVRUSART(cpu,avr.usart0Config,16e6);
let msg='', distance=150, edges=[], leds=[];
uart.onLineTransmit=s=>msg=s.trim();
pd.setPin(2,true); pd.setPin(3,true);
pd.addListener((v,o)=>{
  if((o&64)&&!(v&64)&&distance!==null){
    const width=distance*58*16;
    cpu.addClockEvent(()=>pd.setPin(5,true),3200);
    cpu.addClockEvent(()=>pd.setPin(5,false),3200+width);
  }
});
pb.addListener((v,o)=>{
  if((v^o)&2) edges.push(cpu.cycles/16000);
  if((v^o)&28) leds.push((v>>2)&7);
});
function run(ms){const end=cpu.cycles+ms*16000;while(cpu.cycles<end){avr.avrInstruction(cpu);cpu.tick();}}
function check(label,expected,actual,pass){console.log('| '+[label,expected,actual,pass?'PASS':'FAIL'].join(' | ')+' |');if(!pass)process.exitCode=1;}
function field(key){return Number(msg.match(new RegExp('(?:^| )'+key+'=(-?\\d+)'))?.[1]);}
function band(label,expected){run(520);check(label,'band '+expected,msg,field('band')===expected);}
run(650); // Include the OLED power-up interval and the full lamp test.
check('Reset in Standard','LEDs 1, 2, 4 and sound edges, range 100','LED masks '+[...new Set(leds)]+' / '+edges.length+' sound edges / '+field('range'),[1,2,4].every(x=>leds.includes(x))&&edges.length>0&&field('range')===100);
for(const [cm,b,p] of [[150,0,0],[101,0,0],[100,1,1000],[75,2,500],[50,3,250],[25,4,100],[10,5,-1],[5,5,-1]]){
  distance=cm;band(cm+' cm',b);edges=[];run(3400);
  let starts=edges.filter((x,i)=>i&&x-edges[i-1]>10);
  let interval=starts.length>1?Math.round(starts[1]-starts[0]):0;
  let good=p===0?edges.length===0:p<0?edges.length>5000&&starts.length===0:Math.abs(interval-p)<=3;
  check(cm+' cm audio',p===0?'silent':p<0?'continuous':p+' ms',p>0?interval+' ms':edges.length+' edges',good);
}
distance=150;band('Remove target',0);
pd.setPin(2,false);edges=[];let start=cpu.cycles/16000;run(35);
const latency=edges[0]-start;check('IR latency; distant echo','first sound within 26 ms',latency.toFixed(2)+' ms',latency>0&&latency<=26);
band('Confirmed IR plus distant echo',7);distance=null;band('IR plus timeout',7);
pd.setPin(2,true);band('Missing echo',6);
distance=1;band('Sub-minimum echo',6);distance=150;band('Echo recovery',0);
function press(ms){pd.setPin(3,false);run(ms);pd.setPin(3,true);run(350);}
for(const r of [60,30,100]){press(75);check('Short press','range '+r,field('range'),field('range')===r);}
press(1350);check('Long press','on=0',msg,field('on')===0);press(1350);check('Resume','on=1',msg,field('on')===1);
distance=5;edges=[];start=cpu.cycles/16000;run(80);let dt=edges[0]-start;check('Step 150 to 5 cm','first sound within 70 ms',dt.toFixed(2)+' ms',dt>0&&dt<=70);
distance=11;band('Retreat 10 to 11 cm',5);
distance=13;band('Retreat beyond 2 cm margin',4);
distance=150;band('Clear before mode tests',0);
press(75);distance=60;band('Indoor at 60 cm',2);distance=61;band('Indoor at 61 cm',0);
press(75);distance=30;band('Precision at 30 cm',3);distance=31;band('Precision at 31 cm',0);
pd.setPin(2,false);run(5);pd.setPin(2,true);band('Ignore 5 ms IR disturbance',0);

const before=ramWrites;
run(550);
check("Steady OLED", "no redundant writes", ramWrites-before, ramWrites===before);
distance=7;
run(600);
check("Changed OLED", "data written after distance change", ramWrites-before, ramWrites>before);
