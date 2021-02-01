const fsevents = require('fsevents')
const waitForSigint = require('wait-for-sigint');
const fs = require("fs");

if (process.argv.length < 3) {
  console.error("Usage: node watch.js <path>");
  process.exit(1);
}

const path = process.argv[2];
if (!fs.existsSync(path)) {
  console.error(`Not found: ${path}`);
  process.exit(1);
}

console.log(`Monitoring ${path}`);

const stop = fsevents.watch(path, (p, f, id) => {
  const info = fsevents.getInfo(p, f, id);
  console.log(`[${info.event} ${info.type}] ${info.path}`);
});

waitForSigint().then(stop);
