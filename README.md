# Drogon Jsonrpc

Use jsonrpc in drogon with both http and websocket.

## Usage

Add service files in src/services, referring to HelloWorldService.cpp.

## Examples

```
$ curl localhost:8080/jsonrpc -X POST -H "Content-Type: application/json" -d '{"jsonrpc": "2.0", "id": "1", "method": "hello_world", "params": {}}'
{"jsonrpc":"2.0","id":"1","result":{"message":"hello world","status":"ok"}}

curl localhost:8080/jsonrpc -X POST -H "Content-Type: application/json" -d '{"jsonrpc": "2.0", "id": "2", "method": "bad", "params": {}}'
{"jsonrpc":"2.0","id":"2","error":{"code":-1000,"message":"This is bad!"}}

```
