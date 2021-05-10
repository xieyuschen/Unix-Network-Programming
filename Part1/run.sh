sudo rm client server
gcc -I../../unpv13e/lib -I../Part1 client.c -w -o  client
gcc -I../../unpv13e/lib -I../Part1 server.c -w -o  server