typedef struct
{
	PBlock pb[10];
	FValue fv[10];
	SubHeader1 sh1;
	SubHeader2 sh2;
} MCU;

typedef struct
{
	PBlock pb[10];
	SubHeader1 sh1;
	SubHeader2 sh2;
} MCU2 ;


typedef struct
{
	FValue fv[10];
	SubHeader1 sh1;
	SubHeader2 sh2;
} FF12;

typedef struct
{
	FValue fv[10];
	SubHeader1 sh1;
	SubHeader2 sh2;
} FF32;

typedef struct
{
	PBlock pb[10];
	SubHeader1 sh1;
	SubHeader2 sh2;
} FF23 ;

typedef struct
{
	PBlock pb[10];
	SubHeader1 sh1;
	SubHeader2 sh2;
} FF21 ;

typedef struct
{
	SubHeader1 sh1;
	SubHeader2 sh2;
} FF13 ;
