#define TABLELEN        63
#define BUFFFERLEN      181

#define ENCODERLEN      4
#define ENCODEROPLEN    0
#define ENCODERBLOCKLEN 3

#define PADDINGCHAR     '='
#define BASE64CHARSET   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
                        "abcdefghijklmnopqrstuvwxyz"\
                        "0123456789"\
                        "+/";
/* Function prototypes */
int Base64Encode(char *input, char *output, int oplen);
int encodeblock(char *input, char *output, int oplen);
int Base64Decode(char *input, char *output, int oplen);
int decodeblock(char *input, char *output, int oplen);
