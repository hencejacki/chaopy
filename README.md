# Chaopy

## Aim To

Monitor the state of file under `path` and **chaopy** them to `target` path as necessary.

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
			"path": "E:/AWorkSpace/Chaopy/bin/Debug",
			"target": "E:/AWorkSpace/Chaopy/test",
			"recursive": true
		}
	]
}
~~~

2. Start program.

## TODO

- [x] Configuration parse.
- [ ] Core functionality implement.
