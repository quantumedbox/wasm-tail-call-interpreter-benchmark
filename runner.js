WebAssembly.instantiateStreaming(fetch("tail-call.wasm")).then(
  (obj) => {
    console.time('tail-call.wasm (cold)');
    obj.instance.exports.main();
    console.timeEnd('tail-call.wasm (cold)');
  },
);

WebAssembly.instantiateStreaming(fetch("jump-table.wasm")).then(
  (obj) => {
    console.time('jump-table.wasm (cold)');
    obj.instance.exports.main();
    console.timeEnd('jump-table.wasm (cold)');
  },
);
