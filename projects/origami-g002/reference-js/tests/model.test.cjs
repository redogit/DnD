const assert=require('node:assert/strict');
const fs=require('node:fs');
const path=require('node:path');
const file=path.join(__dirname,'../lib/model.js');
assert.ok(fs.existsSync(file),'A standalone deterministic mathematical model is required');
const m=require(file);
// A fixed point must remain fixed; sequential coordinate updates corrupt integration.
assert.deepEqual(m.step('lorenz',[0,0,0]),[0,0,0]);
const h=.01,x=.1;
const actual=m.step('lorenz',[x,0,0]);
// Independent high-resolution Euler reference to the continuous Lorenz equations.
let ref=[x,0,0];
for(let i=0;i<10000;i++){const [a,b,c]=ref,dt=h/10000;ref=[a+10*(b-a)*dt,b+(a*(28-c)-b)*dt,c+(a*b-8*c/3)*dt];}
actual.forEach((v,i)=>assert.ok(Math.abs(v-ref[i])<2e-7,`RK4 coordinate ${i}`));
for(const feed of ['lorenz','rossler','clifford']){let state=[.1,0,0];for(let i=0;i<20000;i++)state=m.step(feed,state);assert.ok(state.every(Number.isFinite),feed+' remains finite');}
assert.equal(m.targets(0).frequency,2.4);
assert.equal(m.targets(90).frequency,5.8);
assert.equal(m.targets(90).polarization,90);
assert.throws(()=>m.targets(91),RangeError);
assert.throws(()=>m.step('unknown',[0,0,0]),RangeError);
console.log('PASS: fixed point, independent integration reference, 60,000 stability steps, target boundaries and input guards');
