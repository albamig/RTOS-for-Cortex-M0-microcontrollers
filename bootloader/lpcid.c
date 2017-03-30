// from "lpc2isp" by Martin Maurer (Martin.Maurer@clibb.de)

typedef enum  {
  CHIP_VARIANT_NONE,
  CHIP_VARIANT_LPC2XXX,
  CHIP_VARIANT_LPC17XX,
  CHIP_VARIANT_LPC13XX,
  CHIP_VARIANT_LPC11XX
} CHIP_VARIANT;

typedef struct{
    const unsigned long  id;
    const char 			*Product;
    const unsigned int   FlashSize;     /* in kiB, for informational purposes only */
    const unsigned int   RAMSize;       /* in kiB, for informational purposes only */
          unsigned int   FlashSectors;  /* total number of sectors */
          unsigned int   MaxCopySize;   /* maximum size that can be copied to Flash in a single command */
    const unsigned int  *SectorTable;   /* pointer to a sector table with constant the sector sizes */
    const CHIP_VARIANT   ChipVariant;
} LPC_DEVICE_TYPE;

static const unsigned int SectorTable_210x[] =
{
    8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
    8192, 8192, 8192, 8192, 8192, 8192, 8192
};

static const unsigned int SectorTable_2103[] =
{
    4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096
};

static const unsigned int SectorTable_2109[] =
{
    8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192
};

static const unsigned int SectorTable_211x[] =
{
    8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
    8192, 8192, 8192, 8192, 8192, 8192, 8192,
};

static const unsigned int SectorTable_212x[] =
{
    8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
    65536, 65536, 8192, 8192, 8192, 8192, 8192, 8192, 8192
};

// Used for devices with 500K (LPC2138 and LPC2148) and
// for devices with 504K (1 extra 4k block at the end)
static const unsigned int SectorTable_213x[] =
{
     4096,  4096,  4096,  4096,  4096,  4096,  4096,  4096,
    32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
    32768, 32768, 32768, 32768, 32768, 32768,  4096,  4096,
     4096,  4096,  4096,  4096
};

// Used for LPC17xx devices
static const unsigned int SectorTable_17xx[] =
{
     4096,  4096,  4096,  4096,  4096,  4096,  4096,  4096,
     4096,  4096,  4096,  4096,  4096,  4096,  4096,  4096,
    32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768,
    32768, 32768, 32768, 32768, 32768, 32768
};


static const LPC_DEVICE_TYPE LPCtypes[] =
{

   // id, name of product, flash size, ram size, total number of sector, max copy size, sector table, chip variant

   { 0x2500102B, "1102",         32,  8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },

   { 0x041E502B, "1111.../101",   8,  2,  2, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x2516D02B, "1111.../102",   8,  2,  2, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0416502B, "1111.../201",   8,  4,  2, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x2516902B, "1111.../202",   8,  4,  2, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x042D502B, "1112.../101",  16,  2,  4, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x2524D02B, "1112.../102",  16,  2,  4, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0425502B, "1112.../201",  16,  4,  4, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x2524902B, "1112.../202",  16,  4,  4, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0434502B, "1113.../201",  24,  4,  6, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x2532902B, "1113.../202",  24,  4,  6, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0434102B, "1113.../301",  24,  8,  6, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x2532102B, "1113.../302",  24,  8,  6, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0444502B, "1114.../201",  32,  4,  8, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x2540902B, "1114.../202",  32,  4,  8, 1024, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0444102B, "1114.../301",  32,  8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x2540102B, "1114.../302",  32,  8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },

   { 0x1421102B, "11C12.../301", 16,  8,  4, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x1440102B, "11C14.../301", 32,  8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x1431102B, "11C22.../301", 16,  8,  4, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x1430102B, "11C24.../301", 32,  8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },

   { 0x0364002B, "1224.../101",  32,  8,  4, 2048, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0364202B, "1224.../121",  48, 12, 32, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0365002B, "1225.../301",  64, 16, 32, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0365202B, "1225.../321",  80, 20, 32, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0366002B, "1226",         96, 24, 32, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },
   { 0x0367002B, "1227",        128, 32, 32, 4096, SectorTable_17xx, CHIP_VARIANT_LPC11XX },

   { 0x2C42502B, "1311",          8,  4,  2, 1024, SectorTable_17xx, CHIP_VARIANT_LPC13XX },

   { 0x1816902B, "1311/01",       8,  4,  2, 1024, SectorTable_17xx, CHIP_VARIANT_LPC13XX },

   { 0x2C40102B, "1313",         32,  8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC13XX },

   { 0x1830102B, "1313/01",      32,  8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC13XX },

   { 0x3D01402B, "1342",         16,  4,  4, 1024, SectorTable_17xx, CHIP_VARIANT_LPC13XX },

   { 0x3D00002B, "1343",         32,  8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC13XX },


   { 0x25001118, "1751",         32,  8,  8, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x25001121, "1752",         64, 16, 16, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x25011722, "1754",        128, 32, 18, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x25011723, "1756",        256, 32, 22, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x25013F37, "1758",        512, 64, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x25113737, "1759",        512, 64, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x26011922, "1764",        128, 32, 18, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x26013733, "1765",        256, 64, 22, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x26013F33, "1766",        256, 64, 22, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x26012837, "1767",        512, 64, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x26013F37, "1768",        512, 64, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x26113F37, "1769",        512, 64, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },

   { 0x27011132, "1774",        128, 40, 18, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x27191F43, "1776",        256, 80, 22, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x27193747, "1777",        512, 96, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x27193F47, "1778",        512, 96, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x281D1743, "1785",        256, 80, 22, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x281D1F43, "1786",        256, 80, 22, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x281D3747, "1787",        512, 96, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },
   { 0x281D3F47, "1788",        512, 96, 30, 4096, SectorTable_17xx, CHIP_VARIANT_LPC17XX },

   { 0x0004FF11, "2103",         32,  8,  8, 4096, SectorTable_2103, CHIP_VARIANT_LPC2XXX },
   { 0xFFF0FF12, "2104",        128, 16, 15, 8192, SectorTable_210x, CHIP_VARIANT_LPC2XXX },
   { 0xFFF0FF22, "2105",        128, 32, 15, 8192, SectorTable_210x, CHIP_VARIANT_LPC2XXX },
   { 0xFFF0FF32, "2106",        128, 64, 15, 8192, SectorTable_210x, CHIP_VARIANT_LPC2XXX },
   { 0x0201FF01, "2109",         64,  8,  8, 4096, SectorTable_2109, CHIP_VARIANT_LPC2XXX },
   { 0x0101FF12, "2114",        128, 16, 15, 8192, SectorTable_211x, CHIP_VARIANT_LPC2XXX },
   { 0x0201FF12, "2119",        128, 16, 15, 8192, SectorTable_211x, CHIP_VARIANT_LPC2XXX },
   { 0x0101FF13, "2124",        256, 16, 17, 8192, SectorTable_212x, CHIP_VARIANT_LPC2XXX },
   { 0x0201FF13, "2129",        256, 16, 17, 8192, SectorTable_212x, CHIP_VARIANT_LPC2XXX },
   { 0x0002FF01, "2131",         32,  8,  8, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0002FF11, "2132",         64, 16,  9, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0002FF12, "2134",        128, 16, 11, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0002FF23, "2136",        256, 32, 15, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0002FF25, "2138",        512, 32, 27, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0402FF01, "2141",         32,  8,  8, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0402FF11, "2142",         64, 16,  9, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0402FF12, "2144",        128, 16, 11, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0402FF23, "2146",        256, 40, 15, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0402FF25, "2148",        512, 40, 27, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0301FF13, "2194",        256, 16, 17, 8192, SectorTable_212x, CHIP_VARIANT_LPC2XXX },
   { 0x0301FF12, "2210",          0, 16,  0, 8192, SectorTable_211x, CHIP_VARIANT_LPC2XXX }, /* table is a "don't care" */
   { 0x0401FF12, "2212",        128, 16, 15, 8192, SectorTable_211x, CHIP_VARIANT_LPC2XXX },
   { 0x0601FF13, "2214",        256, 16, 17, 8192, SectorTable_212x, CHIP_VARIANT_LPC2XXX },
   /*            "2290"; same id as the LPC2210 */
   { 0x0401FF13, "2292",        256, 16, 17, 8192, SectorTable_212x, CHIP_VARIANT_LPC2XXX },
   { 0x0501FF13, "2294",        256, 16, 17, 8192, SectorTable_212x, CHIP_VARIANT_LPC2XXX },
   { 0x1600F701, "2361",        128, 34, 11, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1600FF22, "2362",        128, 34, 11, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0603FB02, "2364",        128, 34, 11, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX }, /* From UM10211 Rev. 01 -- 6 July 2007 */
   { 0x1600F902, "2364",        128, 34, 11, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1600E823, "2365",        256, 58, 15, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0603FB23, "2366",        256, 58, 15, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX }, /* From UM10211 Rev. 01 -- 6 July 2007 */
   { 0x1600F923, "2366",        256, 58, 15, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1600E825, "2367",        512, 58, 15, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0603FB25, "2368",        512, 58, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX }, /* From UM10211 Rev. 01 -- 6 July 2007 */
   { 0x1600F925, "2368",        512, 58, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1700E825, "2377",        512, 58, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x0703FF25, "2378",        512, 58, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX }, /* From UM10211 Rev. 01 -- 6 July 2007 */
   { 0x1600FD25, "2378",        512, 58, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX }, /* From UM10211 Rev. 01 -- 29 October 2007 */
   { 0x1700FD25, "2378",        512, 58, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1700FF35, "2387",        512, 98, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX }, /* From UM10211 Rev. 03 -- 25 August 2008 */
   { 0x1800F935, "2387",        512, 98, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1800FF35, "2388",        512, 98, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1500FF35, "2458",        512, 98, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1600FF30, "2460",          0, 98,  0, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1600FF35, "2468",        512, 98, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1701FF30, "2470",          0, 98,  0, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0x1701FF35, "2478",        512, 98, 28, 4096, SectorTable_213x, CHIP_VARIANT_LPC2XXX },
   { 0, "????", 0, 0, 0, 0, 0, CHIP_VARIANT_NONE },  /* unknown */

};

