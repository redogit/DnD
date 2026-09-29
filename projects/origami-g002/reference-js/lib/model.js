(function(root){
'use strict';
const derivatives={lorenz:([x,y,z])=>[10*(y-x),x*(28-z)-y,x*y-8*z/3],rossler:([x,y,z])=>[-y-z,x+.2*y,.2+z*(x-5.7)]};
function step(feed,s){
 if(!Array.isArray(s)||s.length!==3||!s.every(Number.isFinite))throw new TypeError('State requires three finite coordinates');
 if(feed==='clifford'){const [x,y]=s,nx=Math.sin(-1.4*y)+Math.cos(-1.4*x),ny=Math.sin(1.6*x)+.7*Math.cos(1.6*y);return[nx,ny,nx*ny*.5];}
 const f=derivatives[feed];if(!f)throw new RangeError('Unknown mathematical feed');
 const h=feed==='lorenz'?.005:.01,add=(a,b,k)=>a.map((v,i)=>v+k*b[i]);
 for(let half=0;half<2;half++){
 const k1=f(s),k2=f(add(s,k1,h/2)),k3=f(add(s,k2,h/2)),k4=f(add(s,k3,h));
 s=s.map((v,i)=>v+h*(k1[i]+2*k2[i]+2*k3[i]+k4[i])/6);
 }return s;
}
function targets(theta){if(!Number.isFinite(theta)||theta<0||theta>90)throw new RangeError('Fold angle must be 0–90 degrees');const s=Math.sin(theta*Math.PI/180);return{frequency:2.4+3.4*s,polarization:90*s};}
const api={step,targets};if(typeof module==='object'&&module.exports)module.exports=api;else root.OrigamiModel=api;
})(globalThis);
