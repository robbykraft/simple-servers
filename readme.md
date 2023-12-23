# simple web servers on MacOS

simple web servers in different languages. **these are not safe or secure**.

## in C

sources

- [Nir Lichtman](https://www.youtube.com/watch?v=2HrYIl6GpYg)
- [Jeffrey Yu](https://dev.to/jeffreythecoder/how-i-built-a-simple-http-server-from-scratch-using-c-739)

### usage

compile and run server. curl or wget a GET request to localhost:8080/index.html

### notes

these `man` pages on my Mac required number params

- bind: `man 2 bind`
- sockaddr: `man 4 inet`

bind wants type `sockaddr`, but we create a sockaddr_in type, and cast it. [info here](https://stackoverflow.com/questions/21099041/why-do-we-cast-sockaddr-in-to-sockaddr-when-calling-bind)

`htons` will convert a number into hex and swap the endian-ness, for example, 8080 in hex is 1f90, swapping the bytes results in 901f. htons(8080) results in 0x901f.

when sending to a socket, `write` is equivalent to `send` if there are no flags set.
