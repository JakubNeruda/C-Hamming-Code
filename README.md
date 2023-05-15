# C-Hamming-Code
##### Encoder/decoder of (40,32) hamming code capable of recognizing one bit error per block.
Implemented during course PB071.

## Build
```sh
cmake . && make
```

## Usage

### Options
+ `-e "string to encode"` [optional] Encode plaintext to hamming code. Used by default if no option is provided.
+ `-d "string to decode"` [optional] Decode hamming code back to plaintext.

### Encoding
```sh
echo "hello world" > plaintext
cat plaintext | ./hamming > encodedtext
cat encodedtext | xxd
```
```sh
echo -en  "\x00\x01\x02\x03" | ./hamming | xxd
```
### Decoding
```sh
cat encodedtext | ./hamming -d > plaintext
cat plaintext
```
```sh
echo -en  "\x20\x80\x04\x08\x06" | ./hamming -d | xxd
```

## Error correction
There are two 1-bit errors in the following byte string which are corrected and
each correction is noted in stderr.
```shell
echo -en  "\x20\x81\x04\x08\x06\x20\x80\x14\x08\x06" | ./hamming -d | xxd
```

