#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amath.h"
#include "shake.h"
#include "trng.h"
#include "b64.h"
#include "sqlite3.h"
#include "cmd.h"


reg_data  srdata;
reg_data* rdata = &srdata;

sqlite3 *reg_db = 0;
char last_sql_err[256]={0};
int rdata_n=0;


//sql read callback
int srv_idnt_cb(void* arg, int n, char** data ,char** name)
{

 if(n<6) return 0; //check field must me in table
 memset(rdata, 0, sizeof(reg_data));
 rdata_n++;  //count finded db entries
 rdata->n=rdata_n;
 if(data[0]) rdata->Idn=atoi(data[0]);
 if(data[1]) strncpy(rdata->Fio, data[1], sizeof(rdata->Fio));
 if(data[2]) strncpy(rdata->Inn, data[2], sizeof(rdata->Inn));
 if(data[3]) strncpy(rdata->Psw, data[3], sizeof(rdata->Psw));
 if(data[4]) strncpy(rdata->Sec, data[4], sizeof(rdata->Sec));
 if(data[5]) strncpy(rdata->Sig, data[5], sizeof(rdata->Sig));

 //check output mode and callback for set GUI elements
 if(*(int*)arg)
 {
  setlist(rdata); //set data to user's form
 }
 else
 {
  setform(rdata);  //or set data for found list
 }

 return 0;
}

 //open/clode database
 int db_open(char* file)
 {


  int ret=0;
  last_sql_err[0]=0;
  if(file)
  {
   if(sqlite3_open(file, &reg_db))
   {
    reg_db=0;
    return -1;
   }


  }
  else
  {
   if( sqlite3_close(reg_db) )ret=-2;
   else reg_db=0;
  }

  return ret;
 }

 char* db_err(void)
 {
  return last_sql_err;
 }


 //search request for string: field is Fio=0 or Inn=1, mode is match=0,
 //strats=1, contain=2. Returns number of found or error<0
 int db_search(char field, char mode, char* req)
 {

  const char* SQL_REG_SEL = "SELECT * FROM Persons WHERE %s LIKE '%s';";
  signed char str[256]; //string for DB request
  signed char ptr[134]; //pattern for serch
  signed char fld[8]; //field name
  char *err = 0;
  int out=1; //output finded data to user's form
  int ret=0;

  last_sql_err[0]=0;

  if(!reg_db)
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "DB not opened");
   return -2;
  }

  if(field==FIELD_FIO) strncpy(fld, "Fio", sizeof(fld));
  else strncpy(fld, "Inn", sizeof(fld));

  if(!req[0]) strncpy(ptr, "%%", sizeof(ptr)); //SEARCH_ALL
  else if(mode==SEARCH_CONTAIN) snprintf(ptr, sizeof(ptr), "%%%s%%", req);
  else if(mode==SEARCH_START) snprintf(ptr, sizeof(ptr), "%s%%", req);
  else snprintf(ptr, sizeof(ptr), "%s", req); //SEARCH_MATCH

  sprintf(str, SQL_REG_SEL, (char*)fld, (char*)ptr); //SQL DB search requst


  rdata_n=0;
  ret=sqlite3_exec(reg_db, str, srv_idnt_cb, &out, &err);  //search this key in voters DB
  if (ret) //db error
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "Error SQL: %s", err);
   sqlite3_free(err);
   ret=-3;
  }
  else ret=rdata_n;
  
  return ret;
 }

 //read voter's from DB by Id. Returns number of finded or error<0
 int db_read(int n)
 {
  const char* SQL_SEL_IDNT = "SELECT * FROM Persons WHERE Idn=%d;";
  signed char str[256]; //string for DB request
  char *err = 0;
  int out=0; //output finded data to user's form
  int ret=0;

  last_sql_err[0]=0;

  if(!n)
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "Id must be not null");
   return -1;
  }

  if(!reg_db)
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "DB not opened");
   return -2;
  }


  sprintf(str, SQL_SEL_IDNT, n);
  rdata_n=0;
  ret=sqlite3_exec(reg_db, str, srv_idnt_cb, &out, &err);
  if (ret) //db error
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "Error SQL: %sn", err);
   sqlite3_free(err);
   ret=-3;
  }
  else ret=rdata_n;
  return ret;
 }

 //delete voter by Id. Returns 0 on OK or error<0
 int db_delete(int n)
 {
  const char* SQL_DEL_IDNT = "DELETE FROM Persons WHERE Idn=%d;";

  signed char str[256]; //string for DB request
  char *err = 0;
  int ret=0;

  last_sql_err[0]=0;

  if(!n)
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "Id must be not null");
   return -1;
  }

  if(!reg_db)
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "DB not opened");
   return -2;
  }

  sprintf(str, SQL_DEL_IDNT, n);
  ret=sqlite3_exec(reg_db, str, 0, 0, &err);
  if (ret) //db error
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "Error SQL: %sn", err);
   sqlite3_free(err);
   ret=-3;
  }
  return ret;
 }


 //add voters with reg_data. Returns number of last add or error<0
 int db_add(void* p)
 {
  const char* SQL_REG_ADD = "INSERT INTO Persons (Fio, Inn, Psw) VALUES ('%s','%s','%s');";
  const char* SQL_REG_UPD = "UPDATE Persons SET Fio = '%s', Inn = '%s', Psw = '%s' WHERE Idn = %d;";
  signed char str[512]; //string for DB request
  reg_data reg;

  char *err = 0;
  int ret=0;

  last_sql_err[0]=0;

  if(!p)
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "data must be not null");
   return -1;
  }

  if(!reg_db)
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "DB not opened");
   return -2;
  }

  memcpy(&reg, p, sizeof(reg_data));

  if(!reg.Idn) //add new Voter
  {

   sprintf(str, SQL_REG_ADD, reg.Fio, reg.Inn, reg.Psw);
   ret=sqlite3_exec(reg_db, str, 0, 0, &err);
   if (ret)
   {
    snprintf(last_sql_err, sizeof(last_sql_err), "Error SQL: %sn", err);
    sqlite3_free(err);
    ret=-3;
   }
   else ret=sqlite3_last_insert_rowid(reg_db);
   return ret;
  }

  //update existed voter
  sprintf(str, SQL_REG_UPD, reg.Fio, reg.Inn, reg.Psw, reg.Idn);
  ret=sqlite3_exec(reg_db, str, 0, 0, &err);
  if (ret)
  {
   snprintf(last_sql_err, sizeof(last_sql_err), "Error SQL: %sn", err);
   sqlite3_free(err);
   ret=-4;
  }
  else ret=reg.Idn;

  return ret;
 }


  //initialize PRNG
 int initrnd(void)
 {
  short ret=-1;
  short i;

  for(i=0;i<1000;i++)
  {
   ret=trng_init();
   if(ret>=2) break;
  }

  return ret;
 }

 //get random password. Returns password's length if OK or err<0
 int getpas(char* pas, int len)
 {
  char str[32];
  unsigned char rnd[16];
  short i;

  if(len>15) len=15;
  trng_get(rnd, sizeof(rnd));

  b64estr(rnd, sizeof(rnd), str);
  memcpy(pas, str+1, len);
  pas[len]=0;

  return len;
 }
