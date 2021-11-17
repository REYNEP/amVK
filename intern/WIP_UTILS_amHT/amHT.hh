/**
* TODO: Improve Error Messages
* TODO: currently resize() doubles the table and LOAD FACTOR is 50%. Maybe work on that and change some stuffs
* TODO: Choose/Make a better amHT_data structure [maybe a packed like into 12-16 bytes rather than current 24Bytes impl
* TODO: Work on a better [Cache Friendly] HT_data structure. so that user doesn't have to have duplicates of key/values. Or maybe store KEY VAL separately and only store the index in HT_data structure.
* -----------------
* TODO: Expose genHash and hashToIndex to people.... but if someone intends to change those 2 functions, then they will also need to be aware of resize() and m_size
*       So maybe, try to make a Bridge between resize and those Customizable HASH FUNCTIONS.... like some short function (eg. isResizeNeeded, nextTableSize) which USER can manipulate
*       And by MANIPULATE, i litreally mean like they are gonna change the implimentations
* TODO: LOAD_FACTOR Support [currently only m_filledMax is related about this]
* TODO: Make template instead of char* value in amHT_data_15BKey
* TODO: Current Support is only STRINGS .... add INT supports and UNICODE Support [Maybe add some Buttons to Switch Between one another]
* -----------------
* TODO: Now if m_data.rh data type is changed, it is needed to be changed in everywhere in the memFuncs implimentations. Maybe make a Single option, to control all
* TODO: Check all data types that are being used for variables inside functions
* -----------------
* REMEMBER: this is a uint32_t Implimentation
* ALSO: \see WHY UINT32_T section at the bottom of this file
*/
#include "amVK_Logger.hh"
#include <cstring>

/**
 * There's 3 classes amHT_impl, amHT, amHT_noVal
 * USE: amHT or amHT_noVal [amHT_impl is more like under the hood stuffs]
 * amHT_impl functions usually will have '_impl' suffix added to function names.
 * and Most of the time does the Very Neccessary basic stuffs like, Rhood_CollisonSolving, assigning 'key' to m_data.key, but not assigning m_data.val such and such [TODO: Make a List of dif]
 */

namespace amVK_UTILS {
  uint64_t powerOfTwos[33] = {1, 2, 4, 8, 16, 32, 64, 128, 256,
                              512, 1024, 2048, 4096,
                              8192, 16384, 32768, 65536,
                              131072, 262144, 524288, 1048576,
                              2097152, 4194304, 8388608, 16777216,
                              33554432, 67108864, 134217728, 268435456,
                              536870912, 1073741824, 2147483648, 4294967296};

  //amHT_data struct is 24 bytes long. so that makes 96GB for 2^32 sized dict. So we MAX at 24GB [I only have liek 32GB on my machine so... haha]
  const uint32_t amHT_sizeMax = ((0xFFFFFFFF) / 4);                      
  const uint32_t amHT_filledMax = (m_sizeMax / 2);      /** TODO: Maybe, Final Stage Load_Factor is 62.5% [I mean variable Load_Factor] */
  /** TODO-THINK: Considering this limit above, we might wanna switch to int */

  /**
  * \param integer: has to be positive
  * \returns is also an integer either floored or ceiled
  */
  uint8_t floor_log2(uint32_t integer);
  uint8_t floor_log2_divideAndConq(uint32_t integer);
  uint8_t ceil_log2(uint32_t integer);
  /**
  * Simple Crytographic hash Rotation [only 32Bit Integers support]
  */
  uint32_t rotRight(uint32_t hash, uint8_t rotAmount);



  /**
    * |-----------------------------------------|
    *          - amHT data [BUCKET] - WIP [SHORT-TERM: Fix rh type & Support customization]
    * |-----------------------------------------|
  */
  //If you wanna have multiple amHT_data. you might want to take a look at where amHT_data was directly addressed in the function implementations
#ifdef amHT_DATA_15BKEY
  /** TODO: Packed Binary Compiler test */
  /** TODO: Is this one actually using 24 or more BYTES */
  #define amHT_RHOOD_HIGHEST 254
  #define amHT_RHOOD_LOWEST 0
  #define amHT_RHOOD_EMPTY 255
  typedef struct amHT_data_15BKey {
    uint8_t rh;   // [Depends on .rh : rhood_SolveCollision, rhood_Val]
    char key[15];
    char *val;    // [Depends on .val: amHT::resize()]
  } amHT_data;    //24Byte Space [Best Option we Have got I Think]
#elif defined(amHT_DATA_15BKEY_NOVAL)
  #define amHT_RHOOD_HIGHEST 254
  #define amHT_RHOOD_LOWEST 0
  #define amHT_RHOOD_EMPTY 255
  typedef struct amHT_data_15BKey {
    uint8_t rh;   // [Depends on .rh : rhood_SolveCollision, rhood_Val]
    char key[15];
  } amHT_data;    //16Byte Space [Best Option we Have got I Think]
#else
  #define amHT_RHOOD_HIGHEST 127
  #define amHT_RHOOD_LOWEST -127
  #define amHT_RHOOD_EMPTY -1
  typedef struct amHT_data_regular {
    int8_t rh;    //RobinHood Number [Depends on .rh : rhood_SolveCollision, rhood_Val]
    char *key;
    char *val;    //[Depends on .val: amHT::resize()]
  } amHT_data;    //cz of alignment will take 24Byte Space
  //CURRENTLY EVERY FUNCTION is BASED ON THIS STRUCT. will need to upgrade functions based on this Structs change
#endif

  //No Mallocs should be done [stackoverflow.com/a/197699]
#if defined(amHT_DATA_15BKEY)
  #define amHT_data_KEY_ENTRY(index, key)  strcpy(m_data[index].key, key)
#else
  #define amHT_data_KEY_ENTRY(index, key)  m_data[index].key = key
#endif 
  /**
    * |-----------------------------------------|
    *               - amHT Class -
    * |-----------------------------------------|
    * amHT_RELEASE        : if defined, doesn't add error-checking stuffs (Add this macro once you are sure no amHT errors are Happening)
    *                       (If Errors depends on USER's inputs, then it's recommended that you check out amHT_RELEASE stuffs.... its well documented)
    * amHT_IS_KEY_SUP     : [checks if every letter in the key is supported or not] only usage in amHT::genHash()
    *                       \see genHash docs for which chars are sup
    * amHT_DATA_15BKEY    : amHT_data_15BKey will be choosen instead of amHT_data_regular....
    * 
    * amHT_RHOOD_EMPTY    : If this one is POSITIVE number, then some extra code is executed on every rhood_solveCollision()
    * amHT_RHOOD_CHECK    : If this is Defined, rhood_Val()'s output will be checked [if something is wrong] even in amHT_RELEASE 
    * -----------------------------------------
    * HASING-RULES: amHT is CASE_SENSITIVE
    * DOCS: see amHT.cpp
  */
  //CONTAINS PURE-VIRTUAL FUNCTION
  class amHT_impl {
   public:
    //VARIABLES
    amHT_data *m_data = nullptr;
    uint32_t m_size   = 64;       //if index ranges from 0-63, m_size = 64 [amHT chooses 64 for any DICT by default]
    uint32_t m_filled = 0;
    int8_t m_rotAmount = 2;   //Value of this could be worked on, i mean set based on some more thinking, ig.... haha

    /*                BENCHMARKS              */
    //analyzeCollision(void);

    /*                  Hash                  */
    uint32_t genHash(char *key, uint8_t *cnt);  //USES m_rotAmount variable [PRIVATE]
    uint32_t hashToIndex(uint32_t hash);


    /*   [CONSTRUCTOR, DESTRUCTOR & RESIZE]   */
    amHT_impl(uint32_t sizeHint);
    ~amHT_impl();
    bool isResizeNeeded(void);  //based on m_filled
    /**
     * Only way to call resize();
     */
    bool resizeIfNeeded_AndCan(uint32_t new_entries_count);
    /**
    * PURE-VIRTUAL FUNCTION
    * \return true if table has not reached amHT_impl's current highest size support
    */
   private:
    virtual void resize(uint32_t new_entries_count) = 0;
    

    /* [ADD, DELETE & FIND Base]  */
   public:
    /**
    * this function only does Base Stuffs, which are must for having a working amHT
    * \note doesn't check for resize()
    * \see Docs in amHT::add()
    * \return false if added previously
    * \param index: you should pass a pointer to a var. Var will be set to usable bucket index for DICT ENTRY
    */
    bool addDuplicate_impl(char *key, uint32_t *index);
    /**
    * FASTER Implimentation + use when key is NoDuplicate
    * \note doesn't check for resize()
    * \see amHT::addMultiple_NoDuplicate()  [1reason this func doesn't do m_filled++]
    * \param index: you should pass a pointer to a var. Var will be set to usable bucket index for DICT ENTRY
    */                  
    void addRaw_impl(char *key, uint32_t *index);

    /**
     * \param index: you should pass a pointer to a var. if \return false: Var will be set to usable bucket index for DICT ENTRY
     *                                                   if \return true:  Val will be the index where the match was found....
     */
    bool find_impl(char *key, uint32_t *index);

   protected:
    /*             - [ROBIN HOOD] -            */
    uint32_t rhood_SolveCollision(uint32_t index);
    int8_t rhood_Val(uint32_t index_from, uint32_t index_to);   //Return the distance between 2 params [a.k.a rh value (amHT_data.rh)]

    /**
    * TODO: BENCHMARKING PRIVATE STUFFS....
    */
  };

  class amHT : public amHT_impl 
  {
   public:
    /**
    * NOTE: Before calling any of the add functions, make sure that you yourself Malloc the keys and values on heap
    *       if you are using amHT_data_15BKey instead of amHT_data_regular, Then keys will be hard copied
    *       We Don't do any Mallocs....
    * \return false if added previously
    */
    amHT(uint32_t sizeHint) : amHT_impl(sizeHint) {}                      //CONSTRUCTOR
    bool add(char *key, char *value = nullptr);                           //USES pre_addDuplicate_impl()
    void addMultiple_NoDuplicate(uint32_t n, char **keys, char **values); //USES pre_addRaw_impl()
    void addSingle_NoDuplicate(char *key, char *value);                   //USES pre_addRaw_impl()
    bool find(char *key, char *value = nullptr);

   private:
    /**
    * PRIVATE PURE-VIRTUAL FUNCTION
    * \return true if table has not reached amHT_impl's current highest size support
    * \note call using amHT_impl::resizeIfNeeded_AndCan
    */
    void resize(uint32_t new_entries_count);
  };

  class amHT_noVal : public amHT_impl
  {
   public:
    /**
    * NOTE: Before calling any of the add functions, make sure that you yourself Malloc the keys and values on heap
    *       if you are using amHT_data_15BKey instead of amHT_data_regular, Then keys will be hard copied
    *       We Don't do any Mallocs....
    * \return false if added previously
    * Since this is amHT_noVal, there are no value parameters
    */
    amHT_noVal(uint32_t sizeHint) : amHT_impl(sizeHint) {}  //CONSTRUCTOR
    bool add(char *key);                                    //USES pre_addDuplicate_impl()
    void addMultiple_NoDuplicate(uint32_t n, char **keys);  //USES pre_addRaw_impl()
    void addSingle_NoDuplicate(char *key);                  //USES pre_addRaw_impl()
    bool find(char *key);

   private:
    /**
    * PRIVATE PURE-VIRTUAL FUNCTION   :   [It's actually fuunny, cz even if it's a mem func of amHT_noVal, still no mem-funcs can call resize(), even tho amHT_impl funcs can]
    * \return true if table has not reached amHT_impl's current highest size support
    * \note call using amHT_impl::resizeIfNeeded_AndCan
    */
    void resize(uint32_t new_entries_count);
  };
}

/**
 * TODO: MAYBE INSTEAD TRY ENABLING COMPRESSION or SMTN
* WHY UINT32_T
* 2^32 literally means 4 Billion+ 
* and if every bucket in table contains a KEY & VALUE pair + Robin-Hood Hashing (1-2 Bytes)
* KEY & VALUE both will be pointers to memory address, also i think avg. length of those can be considered 8 BYTES too
* That makes total of 34 Bytes. Lets consider a bit higher say 40-50 Bytes per case....
* ppl will also have every (KEY, VALUE) in memory before add() ing to DICT 
* (2Billion * 40 BYTES) + (2Billion * 20 BYTES) = 120Billion = ~112 GBytes [2Billion = 0.5 LoadFactor]
* That means you gotta have 128GB+ Memory to actually have a table that size. I Don't think thats gonna happen even after a Decade from now (28 MAY 2021)
*/