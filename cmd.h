
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include "base32.h"
//#include "qrcode.h"
#include "qrcodegen.h"
#include "amath.h"
#include "jpec.h"
#include "quirc.h"

#define FIELD_FIO 0
#define FIELD_INN 1
#define SEARCH_MATCH 0
#define SEARCH_START 1
#define SEARCH_CONTAIN 2


#define QRCD_I_LEN 4    //user ID
#define QRCD_O_LEN 12   //binary server's onion address
#define QRCD_P_LEN 16   //password

#define QRCD_I 0
#define QRCD_O QRCD_I+QRCD_I_LEN
#define QRCD_P QRCD_O+QRCD_O_LEN
#define QRCD_L QRCD_P+QRCD_P_LEN

#define QRMODULE_N 29 //size of QR-code in modules (n*n)
#define QRMODULE_S 10  //size of elemenary module in pixels

 //client's data from DB
typedef struct
{
  int n;  //count
 int Idn;  //ID
 char Fio[128]; //FIO
 char Inn[16];  //INN
 char Psw[32];  //Pasword
 char Sec[64];  //Shared secret
 char Sig[128]; //Blind signature
}  reg_data;


 //callback for set data in form
 void setform(void* p);


 //callback for set data in list
 void setlist(void* p);

 //get last db error
 char* db_err(void);

 //open/clode database
 int db_open(char* file);


 //search request for string: field is Fio=0 or Inn=1, mode is match=0,
 //strats=1, contain=2. Returns number of found or error<0
 int db_search(char field, char mode, char* str);

 //read voter's from DB by Id. Returns number of finded or error<0
 int db_read(int n);

 //delete voter by Id. Returns 0 on OK or error<0
 int db_delete(int n);

 //add voters with reg_data. Returns number of last add or error<0
 int db_add(void* p);

 //initialize PRNG
 int initrnd(void);

 //get random password. Returns password's length if OK or err<0
 int getpas(char* pas, int len);

// int8_t qrcode_initText(QRCode *qrcode, uint8_t *modules, uint8_t version, uint8_t ecc, const char *data);
//int8_t qrcode_initBytes(QRCode *qrcode, uint8_t *modules, uint8_t version, uint8_t ecc, uint8_t *data, uint16_t length);
//bool qrcode_getModule(QRCode *qrcode, uint8_t x, uint8_t y);


bool qrcodegen_encodeText(const char *text, uint8_t tempBuffer[], uint8_t qrcode[],
	enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl);
bool qrcodegen_encodeBinary(uint8_t dataAndTemp[], size_t dataLen, uint8_t qrcode[],
	enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl);
bool qrcodegen_isAlphanumeric(const char *text);
int qrcodegen_getSize(const uint8_t qrcode[]);
bool qrcodegen_getModule(const uint8_t qrcode[], int x, int y);


jpec_enc_t *jpec_enc_new(const uint8_t *img, uint16_t w, uint16_t h);
const uint8_t *jpec_enc_run(jpec_enc_t *e, int *len);
void jpec_enc_del(jpec_enc_t *e);



void itom(unsigned char *p, unsigned int const a);
unsigned int crc32_le(unsigned char const *p, unsigned int len);
unsigned short telcrc16(unsigned char const *p, int len);
int b64estr(const unsigned char* data, int bytes, char* str);
int b64dstr( const char *str, unsigned char* data, int maxlen );
void base32_encode(char *dest, const unsigned char *src);
int base32_decode(unsigned char *dest, const char *src);


#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif
