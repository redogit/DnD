const assert=require('node:assert/strict'),fs=require('node:fs');
assert.ok(fs.existsSync(__dirname+'/../lib/engineering.js'),'Explicit engineering models are required');
const E=require('../lib/engineering.js');
const dist=(a,b)=>Math.hypot(...a.map((v,i)=>v-b[i]));
const sub=(a,b)=>a.map((v,i)=>v-b[i]);const dot=(a,b)=>a.reduce((t,v,i)=>t+v*b[i],0);const cross=(a,b)=>[a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]];
for(const theta of [0,15,45,75,89.99,90]){const g=E.miura(theta),n=13;for(let j=0;j<n;j++)for(let i=0;i<n;i++){const a=g.vertices[j*(n+1)+i],b=g.vertices[j*(n+1)+i+1],c=g.vertices[(j+1)*(n+1)+i],d=g.vertices[(j+1)*(n+1)+i+1];assert.ok(Math.abs(dist(a,b)-g.a)<1e-10);assert.ok(Math.abs(dist(a,c)-g.b)<1e-10);assert.ok(Math.abs(dot(cross(sub(b,a),sub(c,a)),sub(d,a)))<1e-9);assert.ok(Math.abs(dist(a,d)**2+dist(b,c)**2-2*(g.a*g.a+g.b*g.b))<1e-9);}}
const v=Array.from({length:13},(_,i)=>i/7-1);for(let i=0;i<13;i++)for(let j=i+1;j<13;j++){const r=E.rotate13(v,i,j,73),back=E.rotate13(r,i,j,-73);assert.ok(Math.abs(dot(v,v)-dot(r,r))<1e-12);assert.ok(dist(v,back)<1e-12);}
const collision=[...v];collision[12]+=1;assert.deepEqual(E.project13(v).visible,E.project13(collision).visible);assert.notDeepEqual(E.project13(v).residual,E.project13(collision).residual);
const reson=E.resonance(2.2,.75);assert.ok(Math.abs(reson-3.918123848382)<.001);
for(const f of [2.4,3.5,5.8,7]){const c=E.circuit(f,{capacitance:.75});assert.ok(c.magnitude<=1+1e-10);assert.ok(c.insertionLoss>=-1e-10);assert.equal(c.s21dB,-c.insertionLoss);}
assert.ok(E.circuit(reson,{capacitance:.75}).magnitude<1e-10,'Ideal shunt branch produces a notch at LC resonance');
assert.throws(()=>E.rotate13(v,0,0,30));assert.throws(()=>E.miura(-1));assert.throws(()=>E.circuit(0));
console.log('PASS: 1,014 rigid facets at six fold states, 78 plane rotations/inverses, projection collision/residual, passive RF boundaries and LC notch');
