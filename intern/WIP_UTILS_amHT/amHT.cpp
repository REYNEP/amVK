/**
* Page sections:
*   I: HASH [amHT_impl]
*  II: ROBIN HOOD Funcs [amHT_impl]
* III: CONSTRUCTOR, DESTRUCTOR & RESIZE [amHT_impl]
*  IV: add(), delete() [WIP] & find() BASE impl [amHT_impl]
*  VI: Util FUNCS [amVK_UTILS & amHT_impl]
* VII: amHT
*  IX: amHT_noVal
*   X: Procedural Workshop [Just kidding. Docs on Hashing n IDEAS]
*/
#include "amHT.hh"

//Docs on How this Works is at the Bottom of the PAGE
namespace amVK_UTILS {
uint32_t amHT_impl::genHash(char *key, uint8_t *cnt) {
    /**
    * Checking if any letter is anything other than... ENG-LETTER / _ (underscore) / . (dot)
    */
#if !defined(amHT_RELEASE) || defined(amHT_IS_KEY_SUP)
    for (uint32_t cnt = 0; key[cnt] != '\0'; cnt++) {
        uint32_t c = key[cnt];
        if (((c > 64) && (c < 91)) || ((c > 96) && (c < 113))) {
            continue;
        }
        else if (c == 0 || c == '_' || c == '.') continue;    //Keys that INSTA allows
        else LOG(cnt+1 << "th letter in key [" << key << "] isn't 1-of eng-letters or _ (underscore) or . (dot)"); amABBORT(10);
    }
#endif

    uint32_t hash = 0;
    uint32_t base_2ThePower_cnt = 1;    //52**cnt
    uint8_t i = 0; 
    while (key[i] != '\0') {
        uint32_t c = key[i];
        if ((c > 64) && (c < 91)) c = c - 38; //64 - 26 = 38
        else if ((c > 96) && (c < 113)) c = c - 95;
    
        base_2ThePower_cnt *= 52;
        hash += c * base_2ThePower_cnt;
        hash = amVK_UTILS::rotRight(hash, m_rotAmount);
        i++;
    }

    *(cnt) = i;
    return hash;
}

/**
* m_size will be always multiples of 2
* TODO: See if inline boosts perf.
*/
uint32_t amHT_impl::hashToIndex(uint32_t hash) {
    return hash & m_size;
}







/**
  * |-----------------------------------------|
  *              - [ROBIN HOOD] -
  * |-----------------------------------------|
*/
/**
 * \param index_from: What you have input to rhood_SolveCollision
 * \param index_to: what was returned by rhood_SolveCollision
 */
int8_t amHT_impl::rhood_Val(uint32_t index_from, uint32_t index_to) {
#if defined(amHT_RELEASE) && (!defined(amHT_RHOOD_CHECK))
    if (index_to < index_from) {
        return (int8_t) (0xFFFFFFFF - index_from) + (index_to + 1);
    } else {
        return (int8_t) index_to - index_from;
    }
#else
    int rh = 0;
    if (index_to < index_from) {
        rh = (0xFFFFFFFF - index_from) + (index_to + 1);    //cause index_to starts at 0, but we also have to count that Zero too
    } else {
        rh = index_to - index_from;
    }

    
    if (rh > amHT_RHOOD_HIGHEST) {
        LOG_EX("amHT won't work, [DICT contains more COLLISIONS than CURRENT SUPPORT on a particular INDEX]" << std::endl 
                << "m_data.rh number is BIGGER than it can fit.");
        amABBORT(10);
    } else if (rh < amHT_RHOOD_LOWEST) {
        LOG_EX("amHT won't work, [REPORT THIS BUG] m_data.rh number is SMALLER than it can fit.");
        amABBORT(10);
    }

    return (int8_t) rh;
#endif
}

/**
* Probably the only RECURSIVE function we have
* \return final index where we can actually put stuffs
* if the final_index already has some entry in it, rhood_SolveCollision (recursive) moves it first
* ---------------
* If colliding Entry\ies [which is already there in DICT] originally collided on \param index, then we get past these ones
* instead of sending the first one [Originally Colliding on \param index] to the end like Malte_Skarupke
* If you couldn't understand, I will soon give an Really Demonstraded Example (made with Nodes of this API) 
* But for now, Please read the first 2 Comment-Block inside the Function
*/
uint32_t amHT_impl::rhood_SolveCollision(uint32_t index) {
    /**
    * rOBIN hOOD solves collision by moving colliding entries to the next bucket (even if the next bucket has something in it, in this case that smtn is moved)
    * assigns a rhood number to each entry, eg. if rh number is 7, means this bucket has element which's actual index/place was 7 buckets ago 
    * but even after holding this true, the bucket just before OLD BUCKET (rh = 7), might not be rh = 6, instead rh might be bigger than 6
    * cause that bucket before OLD BUCKET might hold entries which's ORIGINAL-INDEX was smaller/before than what's in our OLD BUCKET
    */

    /** -------- Get past the Buckets occupied by Colliding dict-entries [collides on \param index or before] GG: 'on or before' -------- */
    int how_far_we_are = -1;    //How Far We Are - 1        [-1, bcz otherwise w'ld have to use (>=) in while-Loop below, no OTHER REASON REALLY!!!!]
    while (m_data[index].rh > how_far_we_are) {
#if amHT_RHOOD_EMPTY > 0
        //sometimes if amHT_RHOOD_EMPTY is not -1, so how_far_we_are won't actually work
        if (m_data[index].rh == amHT_RHOOD_EMPTY) {
            return index;
        }
#endif
        
        how_far_we_are++;
        index++;
        //Rotation Stuffs   [TODO: Maybe, remove this & instead make m_size = m_size + sizeof(typeof(m_data.rh))] NOTE: There are other rotation stuffs too
        if (index < m_size) continue;   //eg. if index ranges from 0-63, m_size = 64 [m_size is 1 bigger]
        else index = 0;
    }

    if (m_data[index].rh != amHT_RHOOD_EMPTY) {
        //if not (-1) empty, here must be some other stuffs. Need to find a suitable place for whats in here
        uint32_t where_to = amHT_impl::rhood_SolveCollision(index);

        // and MOVE it
        m_data[where_to] = m_data[index];
        m_data[where_to].rh = rhood_Val(index, where_to);

        return index;
    } 
    else {  //m_data[index] is empty
        return index;
    }
}


































/**
  * |-----------------------------------------|
  *   - [CONSTRUCTOR, DESTRUCTOR & RESIZE] -
  * |-----------------------------------------|
  * I: LOAD_FACTOR  [WIP]
  * II: HIGHEST TABLE-SIZE Maintanance
  * III: RESIZING   [has an add function in add section]
  * IV: Wipes out KEY & VALS
  * V: Creation of amHT_impl Dictionaries ;)
*/
/**
* Allocated the actual Dict on the HEAP
* \param sizeHint: you can hint about the size by telling How many ENTRIES (a.k.a nonDuplicate add()) you are gonna make
*/
amHT_impl::amHT_impl(uint32_t sizeHint) {
    /** -------- ERROR Checking -------- */
    if (sizeHint < 64) {
        m_size = 64;
    }
    else if (sizeHint > amHT_filledMax) { //check amVK_UTILS namespace variables in amHT_impl.hh file
        LOG("amHT_impl doesn't have support for this many Elements.");
        amABBORT(10);
    } 
    else {
        uint8_t log2_size = amVK_UTILS::ceil_log2(sizeHint);
        m_size = (uint32_t) amVK_UTILS::powerOfTwos[log2_size+1];   //Load_Factor is 50% + \see powerOfTwos in amHT_impl.hh
    }

    /** -------- KEY STUFFS -------- */
    if (m_size > powerOfTwos[22]) { // (2^22)
        m_rotAmount = 4;
    } //else default m_rotAmount = 2;
    m_data = new amHT_data[m_size];
}
/**
* Not called Internally.
* \warning this will delete everything, complete clean everything related to this dict. 
*          eg. addSingle_Duplicate sometimes mallocs keys and values, those will be gone too
*/
amHT_impl::~amHT_impl() {
    delete m_data;
}

/**
* Current Load Factor is 50%. 
* so \return true if m_filled > (m_size / 2)
*/
bool amHT_impl::isResizeNeeded(void) {
    if (m_filled > (m_size / 2)) {
        return true;
    } else {
        return false;
    }
}
bool amHT_impl::resizeIfNeeded_AndCan(uint32_t new_entries_count) {
    uint32_t new_req_size = m_filled + new_entries_count;
    uint8_t log2_size = amVK_UTILS::ceil_log2(new_req_size);
    uint32_t newTable_size = (uint32_t) amVK_UTILS::powerOfTwos[log2_size+1];   //Load_Factor is 50% + \see powerOfTwos in amHT_impl.hh
    if (newTable_size > amHT_sizeMax) {
        LOG_EX("Can't Resize furthur. Reached amHT_maxSize. Returning False");
        return false;
    } else if (newTable_size == this->m_size) {
        LOG_EX("Resize not needed. Returning false");
        return false;
    } 

    //else
    amHT_impl::resize(new_entries_count);
    return true;
}


































/**
  * |-----------------------------------------|
  *     - [ADD, DELETE & FIND Base Impl] -
  * |-----------------------------------------|
*/
/**
* this function only does the Common Pre tasks for add() or a.k.a DICT-ENTRY
* \note doesn't check for resize()
* \see Docs in amHT::add()
* \return false if added previously
* \param index: you should pass a pointer to a var. Var will be set to usable bucket index for DICT ENTRY
*/
bool amHT_impl::addDuplicate_impl(char *key, uint32_t *index) {
    uint8_t key_len = 0;
    uint32_t hash = amHT_impl::genHash(key, &key_len);
    uint32_t index_actual = amHT_impl::hashToIndex(hash);
    uint32_t i = index_actual;
    bool isDuplicate = amHT_impl::find_impl(key, &i);  //i is now the bucket we can actually use (a.k.a final_index)

    if (isDuplicate) {
        return false;
    } else {
        //Increase the count and resize if needed (you can see amHT_RESIZE_IF_NEEDED(x) macro in amHT.hh)
        m_filled++; if (amHT_impl::isResizeNeeded()) m_filled--; amHT_impl::resizeIfNeeded_AndCan(1); index_actual = amHT_impl::hashToIndex(hash); i = index_actual;

        // amHT_impl::find() gave us an index where we can put \param key & \param value, 
        if (m_data[i].rh != amHT_RHOOD_EMPTY) {
            // But if there's already smtn, we've to move it first [and xd is 'where' to]
            uint32_t xd_where_to = amHT_impl::rhood_SolveCollision(i);
            int8_t xd_rh = amHT_impl::rhood_Val(i, xd_where_to);    //see docs of this function if confused
            m_data[xd_where_to] = m_data[i];
            m_data[xd_where_to].rh = xd_rh;
        }

        amHT_data_KEY_ENTRY(i, key);    //This is a macro from amHT.hh
        m_data[i].rh = amHT_impl::rhood_Val(index_actual, i);
        
        *(index) = i;
        return true;
        // We do        m_data[i].val = value;        in amHT::add() but not in amHT_noVal::add()
    }
}

/**
* FASTER Implimentation + use when key is NoDuplicate
* \note doesn't check for resize()
* \see amHT::addMultiple_NoDuplicate()  [1reason this func doesn't do m_filled++]
* \param index: you should pass a pointer to a var. Var will be set to usable bucket index for DICT ENTRY
*/
void amHT_impl::addRaw_impl(char *key, uint32_t *index) {
    uint8_t key_len = 0;
    uint32_t hash = amHT_impl::genHash(key, &key_len);
    uint32_t index_actual = amHT_impl::hashToIndex(hash);
    uint32_t i = index_actual;

    int8_t rh = 0;  //rhood value
    if (m_data[i].rh != amHT_RHOOD_EMPTY) {
        i = amHT_impl::rhood_SolveCollision(i);
        rh = amHT_impl::rhood_Val(index_actual, i);    //see docs of this function if confused
    }

    amHT_data_KEY_ENTRY(i, key);    //This is a macro from amHT.hh
    m_data[i].rh = rh;
    *(index) = i;

    // We do        m_data[i].val = value;        in amHT::add() but not in amHT_noVal::add()
}



/**
* NOTE: Matches key only
* \return true if found a match
*/
bool amHT_impl::find_impl(char *key, uint32_t *index) {
    int i = *(index);

    int how_far_we_are = 0;
    while (m_data[i].rh > how_far_we_are) {
        //Get past the Buckets occupied by Colliding dict-entries [collides on indices before \param i]
        how_far_we_are++;

        //Rotation Stuffs
        i++;
        if (i < m_size) continue;
        else i = 0;
    }

    //now rh will be equal to how_far_we_are as far as entries have ORIGINAL INDEX of what was intially passed in to this function's \param i
    while (m_data[i].rh == how_far_we_are) {
        if ((m_data[i].key == key) || strcmp(m_data[i].key, key)) {
            *(index) = i;
            return true;
        }
        how_far_we_are++;

        //Rotation Stuffs
        i++;
        if (i < m_size) continue;
        else i = 0;
    }

    *(index) = i;
    return false;   //could not find any match
}




































/**
  * |-----------------------------------------|
  *             - [Util FUNCS] -
  * |-----------------------------------------|
*/
uint32_t amVK_UTILS::rotRight(uint32_t hash, uint8_t rotAmount) {   //rotAmount should range from 1-31 [tho 0, 32 would be valid too but woudln't make any sense]
    /**
    * TODO: Math Expr. to NodeGraph
    */
#ifndef amHT_RELEASE
    if (rotAmount > 32) {
        LOG("called amVK_UTILS::rotRight() and rotAmount parameter was bigger than 32");
        amABBORT(10);
    }
#endif
    return ((hash & (0xFFFFFFFF >> (32-rotAmount))) << (32-rotAmount)) | (hash >> rotAmount);
}

/**
* \param integer: has to be positive
*/
uint8_t amVK_UTILS::floor_log2(uint32_t integer) {
    //if (integer >= powerOfTwos[32]) amABBORT(10); //Gonna get clamped to (powerOfTwos[32] - 1) anyway because of integer's type being [uin32_t] 

    uint32_t k = 31;  //a.k.a [Integer_Bits_Count] Current Support 32 [So this variable would go upto 31]
    while (true) {
        if (((integer >> k) & 0x1) == 0x1) {    /**if \param integer is say 2^11, then this condition will be true when k will be 10 */
            return k;
        }
        k--;
    }
}
uint8_t amVK_UTILS::floor_log2_divideAndConq(uint32_t integer) {
    uint8_t k = 16;
    uint8_t last = 8;
    while (true) {
        uint32_t shifted_k = integer >> k;

        if ((shifted_k & 0x1) == 0x1) {
            return k;
        }
        else if ((shifted_k | 0x1) > 0x1) {
            k += last;
            last /= 2;
        } 
        else {
            k -= last;
            last /= 2;
        }
    }
}
uint8_t amVK_UTILS::ceil_log2(uint32_t integer) {
    if (integer == 0) return 0;
    else if (integer == 1) return 1;   //cz we ceil_log2

    uint8_t k = amVK_UTILS::floor_log2_divideAndConq(integer);
    if ((((uint32_t)(amVK_UTILS::powerOfTwos[k+1] - 1)) & integer) > (uint32_t)(amVK_UTILS::powerOfTwos[k])) {
        return k + 1;
    } else {
        return k;
    }
}







































/**
  * |-----------------------------------------|
  *             - [amHT FUNCS] -
  * |-----------------------------------------|
*/
bool amHT::find(char *key, char *value) {
    uint8_t key_len = 0;
    uint32_t hash = amHT_impl::genHash(key, &key_len);
    uint32_t index = amHT_impl::hashToIndex(hash);

    return amHT_impl::find_impl(key, &index);   //only matches key TODO: Maybe support for other stuffs too (IDEA: We could simply start checking 
    // - - - - - - - - - - - - - - - - - - - -   from \param index passed to find_impl (as it has been updated), till RobinHood val corresponds (i mean: .rh == index - index_before_find_impl))
}
bool amHT::add(char *key, char *value) {
    uint32_t index = 0xFFFFFFFF;
    if (!amHT_impl::addDuplicate_impl(key, &index)) {
        return false;
    } else {
        m_data[index].val = value;
        return true;
    }
}
void amHT::addMultiple_NoDuplicate(uint32_t n, char **keys, char **values) {
    amHT_impl::resizeIfNeeded_AndCan(n);   //as NoDuplicate we surely can check this

    uint32_t index;
    for (int i = 0; i < n; i++) {
        amHT_impl::addRaw_impl(keys[i], &index);
        m_data[index].val = values[i];
    }
}
void amHT::addSingle_NoDuplicate(char *key, char *value) {
    amHT_impl::resizeIfNeeded_AndCan(1);   //as NoDuplicate we surely can check this

    uint32_t index;
    amHT_impl::addRaw_impl(key, &index);
    m_data[index].val = value;
}

/**
* PURE-VIRTUAL FUNCTION
* \return true if table has not reached amHT_impl's current highest size support
*/
void amHT::resize(uint32_t new_entries_count) {
    amHT *newTable = new amHT(m_filled + new_entries_count);

    uint32_t newTable_i;
    for (int i = 0; i < m_size; i++) {
        //If you implement something like google's flat_hash_map, where you store some bytes of the hash before hashToIndex. 
        //Then we wouldn't need to generate the hashes again for these entries
        if (m_data[i].rh != amHT_RHOOD_EMPTY) {
            newTable->addRaw_impl(m_data[i].key, &newTable_i);
            newTable->m_data[newTable_i].val = m_data[i].val;
        }
    }
    //used below
    amHT_data *thisTable_m_data = this->m_data;

    //cpy the needed Info
    m_data = newTable->m_data;
    m_size = newTable->m_size;
    m_rotAmount = newTable->m_rotAmount;

    //Now newTable will get entirely removed from memory
    newTable->m_data = thisTable_m_data;
    delete newTable;
}



/**
  * |-----------------------------------------|
  *           - [amHT_noVal FUNCS] -
  * |-----------------------------------------|
*/
bool amHT_noVal::find(char *key) {
    uint8_t key_len = 0;
    uint32_t hash = amHT_impl::genHash(key, &key_len);
    uint32_t index = amHT_impl::hashToIndex(hash);

    return amHT_impl::find_impl(key, &index);   //only matches key TODO: Maybe support for other stuffs too (IDEA: We could simply start checking 
    // - - - - - - - - - - - - - - - - - - - -   from \param index passed to find_impl (as it has been updated), till RobinHood val corresponds (i mean: .rh == index - index_before_find_impl))
}
bool amHT_noVal::add(char *key) {
    uint32_t index = 0xFFFFFFFF;    //don't actually need, but the func below needs us to give it one
    return amHT_impl::addDuplicate_impl(key, &index);
}
void amHT_noVal::addMultiple_NoDuplicate(uint32_t n, char **keys) {
    amHT_impl::resizeIfNeeded_AndCan(n);   //as NoDuplicate we surely can check this

    uint32_t index;
    for (int i = 0; i < n; i++) {
        amHT_impl::addRaw_impl(keys[i], &index);
    }
}
void amHT_noVal::addSingle_NoDuplicate(char *key) {
    amHT_impl::resizeIfNeeded_AndCan(1);   //as NoDuplicate we surely can check this

    uint32_t index;
    amHT_impl::addRaw_impl(key, &index);
}

/**
* PURE-VIRTUAL FUNCTION
* \return true if table has not reached amHT_impl's current highest size support
*/
void amHT_noVal::resize(uint32_t new_entries_count) {
    amHT_noVal *newTable = new amHT_noVal(m_filled + new_entries_count);

    uint32_t newTable_i;
    for (int i = 0; i < m_size; i++) {
        //If you implement something like google's flat_hash_map, where you store some bytes of the hash before hashToIndex. 
        //Then we wouldn't need to generate the hashes again for these entries
        if (m_data[i].rh != amHT_RHOOD_EMPTY) {
            newTable->addRaw_impl(m_data[i].key, &newTable_i);
        }
    }
    //used below
    amHT_data *thisTable_m_data = this->m_data;

    //cpy the needed info
    m_data = newTable->m_data;
    m_size = newTable->m_size;
    m_rotAmount = newTable->m_rotAmount;

    //Now newTable will get entirely removed from memory
    newTable->m_data = thisTable_m_data;
    delete newTable;
}

} //namespace amVK_UTILS


































/**
* People Says, that taking Power of 2 Gives a Lot of Common Factors for (KEY, TABLE-SIZE) pairs.
* lets say our table size is 16, and our keys are {8, 16, 24, 32.... 800} then all our keys would only collide into 2 BUCKETS at the table
* at first it might seem that in most cases keys are only gonna overlap on Multiples of 2 BUCKETS
* 
* So ppl usually decides to go with Prime number size of TABLE, and then (KEY, TABLE-SIZE) pair would never have Common Factor as they would be Co-Prime
* But I think Power of 2 Will be okay. And I am gonna try my Best to actually make a workaround, so that we could use Power of 2 Sized TABLE
*
* PROPERTIES: LETTERS (26-ENG), 
*             POSITION
*             VOWELS, (HAS-VOWEL, VOWEL-COUNT)
*             WORD-LENGTH
*             MULTIPLE-WORDS (Contains SPACE), WORD-COUNT (EVEN/ODD)
*             IS-SYMMETRY 
*             IS-SHORT (3-5 LETTERS)
*             IS-(2/4/8/16/32)Valued-LETTER (If we multiply any numbers with these we are gonna get Trailing ZEROES in BINARY)
*             
* A: UsE Binary Operators to Randomize the KEY, 
* B: Dont just Convert NUMBER-2-NUMBER KEY
* C: Case-Sensitivity
* Z: The SOLE Reason we are doing this is just because we want to keep doing BINARY-(&) for HASH-2-INDEX
*
* -----------------------------
* MOVE this if below into [EXTENDED Characters SCANNING]
    if (user_key[cnt] == "_" && user_key[cnt+1] != "\0") {
        //eg. STR = VK_NV_ray_tracing, KEY = vnrt
        //TODO: enable prefix removing options [eg. KEY = rt (VK_NV removed)]
    }

  * |-----------------------------------------|
  *                 - [Hash] -
  * |-----------------------------------------|
  NOTE: \param key: MUST be a string (char *) preferred....
    * ENG-LETTERS, _ (underscore) . (dot) accepted only
    * Remember to USE: amHT_RELEASE macro
*/