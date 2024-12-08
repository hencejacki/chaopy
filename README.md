# Chaopy

## Aim To

Monitor the state of file under `path` and **chaopy**(copy or extra operation with script(haven't support yet)) 
them to `target` path as necessary.

## Platform support

- [x] Windows
- [ ] Linux
- [ ] MacOS

## Usage

1. Put a `JSON` file under the root directory of executable file. The content as below:

~~~json
{
	"monitors": [
		{
			"path": "C:\\Example\\Monitor\\Path",
			"target": "C:\\Example\\Target\\Path",
			"recursive": true
		}
	]
}
~~~

> NOTE: we have put an default config file into `Config` directory you can find it under the root directory of program.

2. Start program.

## TODO

- [x] Configuration parse.
- [x] Core functionality implement.
- [x] Monitor changes under target directory;
- [x] Copy things changed;
- [x] Configurable by json file;
- [ ] Full renewal;
- [ ] Filter rules;
- [ ] Extendable scripts support;
