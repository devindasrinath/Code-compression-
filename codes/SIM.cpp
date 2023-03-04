/* On my honor, I have neither given nor received unauthorized aid on this assignment */

#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <cmath>

//we only used standard namespace libraries
using namespace std;

#define DEBUG //debug enabled
#define MAX_MISMATCHES_RESOLVE 4
#define DIC_SIZE 8
#define BITMASK_SIZE 4

typedef enum
{
   RLE_CANT_USE = 0,
   RLE_NOT_USE = 1,
   RLE_USE = 2
}rle_decisions;

uint8_t read_text(string file_name, string text_data[]);
void build_dictionary(uint8_t data_size, string input_text_data[],string dictionary[]);

string rle_comp_format( uint8_t line_count );
string bit_mask_comp_format(uint8_t start_location ,string bit_mask ,  uint8_t dic_line_index );
string single_bit_mismatch_comp_format(uint8_t mismatch_location , uint8_t dic_line_index );
string double_con_bit_mismatch_comp_format( uint8_t start_location , uint8_t dic_line_index );
string double_any_bit_mismatch_comp_format( uint8_t start_location1, uint8_t start_location2 , uint8_t dic_line_index );
string dictionary_matching_comp_format( uint8_t dic_line_index  );
string no_comp_format(string data_line);

uint8_t hamming_dis_cal(string data_line ,string dic_line, uint8_t mismatch_indexes[]);
bool format_check(uint8_t format_to_check , uint8_t hamming_dis , uint8_t mismatch_indexes[]);
uint8_t rle_format_check(string input_text_data[] , uint8_t index, uint8_t size_text_data , uint8_t *p_num_repeats);
void best_format(string data_line , uint8_t hamming_distances[] ,uint8_t all_mismatch_indexes[][5] ,uint8_t selection_details[]);

uint8_t compressing(string input_file_name , string input_text_data[] ,string dictionary[] ,string output_data[] ,uint8_t *input_size);
string compress_data_line(string input_text_data[] , string dictionary[] , uint8_t data_line_index , uint8_t size_input_data );
string bit_mask_genearate(uint8_t hamming_dis , uint8_t mismatch_indexes[]);

string input_text_data[128]= {};
string dictionary[8]={};
string output_text_data[138]= {};
string output_real_text_data[128]= {};
uint8_t input_data_size=0;


/****************************decompressing functions**********************/
uint8_t decompressing(string compressed_file_name , string dictionary[] , string output_decompressed[]);
string output_decompressed_data[128]= {};
string input_dictionary[8]={};

uint8_t direct_match_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index );
uint8_t single_bit_mismatch_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index );
uint8_t double_con_bit_mismatch_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index );
uint8_t double_any_bit_mismatch_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index );
uint8_t bit_mask_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index );
uint8_t no_compression_dec(string compressed_line , string output_decompressed[] , uint8_t index );
uint8_t rle_dec(string compressed_line ,string output_decompressed[] , uint8_t index );
uint8_t str_to_val(string str );
uint8_t char_to_bit(char bit);
char bit_to_char(uint8_t bit);

void write_text(string file_name, string text_data[] , uint8_t length);

int main (int argc, char** argv)
{

   /***********************************compress data*************************/





if(argv[1][0]=='1')
{
#ifdef DEBUG
    cout<<"************************Compression**************************************"<<endl;
    cout<<"========================================================================="<<endl;
#endif
    uint8_t compressed_length = compressing("original.txt" ,input_text_data, dictionary , output_text_data ,&input_data_size);

#ifdef DEBUG
    /****************show dictionary*******************/
    printf("\n***********dictionary*************\n\n");

    for(uint8_t i = 0; i< DIC_SIZE; i++)
    {
        cout<<dictionary[i]<<endl;
    }



    /****************show compressed size ****************/
    printf("\n size of compressed data : %d\n\n",compressed_length );

    /****************show compressed data***********/
    printf("************compressed data***************\n\n" );
    for(uint8_t i = 0; i< compressed_length; i++)
    {
        cout<<output_text_data[i]<<endl;
    }

    /************validate all compressions are correct.**************/
    read_text("compressed.txt" , output_real_text_data );

    printf("\n**********validity check****************\n\n");
    //compare calculated compressions with given compressions.
    for(uint8_t i = 0; i< compressed_length; i++)
    {
        printf("(%d) equality : %d\n",i, output_real_text_data[i] == output_text_data[i]);
    }

    /************************end of compression*************************/
#endif
}

else if(argv[1][0]=='2')
{
#ifdef DEBUG
    /*****************************decompress data**************************/
    cout<<"\n************************Decompression**************************************"<<endl;
    cout<<"========================================================================="<<endl;
#endif
    uint8_t decompressed_length = decompressing("compressed.txt" ,input_dictionary, output_decompressed_data );
#ifdef DEBUG
    /****************show decompressed size ****************/
    printf("\nsize of decompressed data  : %d\n\n" , decompressed_length);

    printf("***********decompressed data ************ \n\n");
    /****************show uncompressed data ****************/
    for(uint8_t i = 0; i< decompressed_length; i++)
    {
        cout<<output_decompressed_data[i]<<endl;
    }

    /************validate all compressions are correct.**************/
    string original_text_data[128]={};
    read_text("original.txt" , original_text_data );
    printf("\n**********validity check****************\n\n");

    for(uint8_t i = 0; i< decompressed_length; i++)
    {
        printf("(%d) equality : %d\n",i, output_decompressed_data[i] == original_text_data[i]);
    }

    /**************************end of decompressing************/
#endif

}
    return 0;

}



/*!
 * @brief Read text file and store 32bit data in string array.
 *
 * ###Example code
 * @code
 *  string input_text_data[128]= {}; //make string array
 *  read_text("original.txt",input_text_data ); //call to function
 * @endcode
 *
 * @param file_name : Name of the text file
 * @param input_text_data : String array.
 * @return input file length(number of 32bit data)
 */
uint8_t read_text(string file_name, string text_data[])
{
    using namespace std;

    ifstream input_file;
    input_file.open(file_name);

    uint8_t index =0;

    //if file is opened
    if ( input_file.is_open() )
    {

        //loop through line by line and store all 32 bits data in a string array.
        while(getline (input_file, text_data[index]))
        {
            string data_line = text_data[index];

            index++;

            if(data_line.length() < 2) //exceptional case
            {
                index--;

            }
        }

        //NOTE: close the text file after end of reading is necessary.
        input_file.close();
    }

    else
    {
        cout << "Couldn't open data file\n";
    }

    return index;
}/*read_text()*/



/*!
 * @brief write an string array to text file
 *
 * ###Example code
 * @code
 *  string output_text_data[128]= {}; //make string array
 *  read_text("original.txt",input_text_data , 100 ); //call to function
 * @endcode
 *
 * @param file_name : Name of the text file
 * @param output_text_data : String array.
 * @param length : length of String array.
 * @return file written is done
 */
void write_text(string file_name, string text_data[] , uint8_t length)
{
    using namespace std;

    ofstream output_file;

    output_file.open(file_name);

    //if file is opened
    if ( output_file.is_open() )
    {

        //loop through line by line and write all 32 bits strings.
        for (int index = 0; index < length; index++) {
            output_file << text_data[index] << "\n";
        }

        //NOTE: close the text file after end of writing is necessary.

        output_file.close();
    }

    else
    {
        cout << "Couldn't open data file\n";
    }

}/*write_text()*/

/*!
 * @brief build dictionary using 8 most frequently used data
 *
 * ###Example code
 * @code
 *   build_dictionary(data_size,input_text_data );
 * @endcode
 *
 * @param data_size : size of the data in 32 bit format
 * @param input_text_data : String array.
 * @param dictionary : string array where store dictionary data.
 * @return None
 */
void build_dictionary(uint8_t data_size, string input_text_data[] , string dictionary[])
{

    uint8_t count_array[255] = {};
    uint8_t index_array[255] = {};
    uint8_t absoloute_index =0;

    for(uint8_t selected_index=0; selected_index<data_size; selected_index++)
    {
        uint8_t repeat_count =0;
        bool previously_checked =false;

        //loop through the data array and count the repeatation
        for(uint8_t compare_index=0; compare_index<data_size; compare_index++)
        {
            if(input_text_data[selected_index] == input_text_data[compare_index])
            {
                //if we checking already checked number then break comparing
                if(selected_index>compare_index)
                {
                    previously_checked = true;
                    break;
                }

                repeat_count++;
            }
        }

        //if we checking already checked number then skip this
        if(previously_checked)
        {
            continue;
        }

        //update index and count arrays
        count_array[absoloute_index] = repeat_count;
        index_array[absoloute_index] = selected_index;
        absoloute_index++;

    }

    uint8_t selected_indexes[DIC_SIZE]= {};

    //loop thorough 8 dictionary indexes
    for(uint8_t dic_index = 0; dic_index<DIC_SIZE; dic_index++)
    {
        uint8_t max_count=0;
        bool already_checked = false;
        uint8_t max_count_index = 0;

        //find max value from frequency array
        for(uint8_t index = 0; index<absoloute_index; index++)
        {
            //missing checking the value which is already checked.

            for(uint8_t check_index=0; check_index<dic_index; check_index++)
            {
                if(selected_indexes[check_index] ==index_array[index] )
                {
                    already_checked= true;
                    break;
                }
            }

            if(already_checked)
            {
                already_checked= false;
                continue;
            }



            //compare and change the max value
            if(count_array[index]>max_count)
            {
                max_count = count_array[index];
                max_count_index = index_array[index];
            }
        }

        //update dictionary
        dictionary[dic_index] = input_text_data[max_count_index];
        selected_indexes[dic_index] = max_count_index;
    }

}/*build_dictionary()*/

/******************compression formats*******************
********************************************************/


/*!
 * @brief Encode using RLE
 * @param line_count : number of consecutive repetitions
 * @return compressed data
 * ###return format
   | 000 | RLE (2 bits) |
   |-----|--------------|
 */
string rle_comp_format( uint8_t line_count )
{
    string compressed_line = "000"  + bitset< 2 >( line_count - 1 ).to_string();
    return compressed_line;
}/*rle_comp_format()*/


/*!
 * @brief Encode using bit masks
 * @param start_location : location of the start of the mismatch(count from left)
 * @param bit_mask : 4 bit bitmask
 * @param dic_line_index : index of the dictionary entry
 * @return compressed data
 * ###return format
 | 001 | Starting location  (5 bits) | Bitmask (4 bits) | Dic. index (3 bits) |
 |-----|:---------------------------:|:----------------:|:-------------------:|
 */
string bit_mask_comp_format(uint8_t start_location ,string bit_mask ,  uint8_t dic_line_index )
{
    string compressed_line = "001"  + bitset< 5 >( start_location ).to_string() + bit_mask + bitset< 3 >( dic_line_index ).to_string();
    return compressed_line;
}/*bit_mask_comp_format()*/


/*!
 * @brief Encode single bit mismatch
 * @param mismatch_location : location of the mismatch(count from left)
 * @param dic_line_index : index of the dictionary entry
 * @return compressed data
 * ###return format
 | 010 | Mismatch location  (5 bits) | Dic. index (3 bits) |
 |-----|:---------------------------:|:-------------------:|
 */
string single_bit_mismatch_comp_format(uint8_t mismatch_location , uint8_t dic_line_index )
{
    string compressed_line = "010"  + bitset< 5 >( mismatch_location ).to_string() + bitset< 3 >( dic_line_index ).to_string();
    return compressed_line;
}/*single_bit_mismatch_comp_format()*/


/*!
 * @brief Encode double consecutive bit mismatches
 * @param start_location : location of the start of the mismatch(count from left)
 * @param dic_line_index : index of the dictionary entry
 * @return compressed data
 * ###return format
 | 011 | Starting location  (5 bits) | Dic. index (3 bits) |
 |-----|:---------------------------:|:-------------------:|
 */
string double_con_bit_mismatch_comp_format( uint8_t start_location , uint8_t dic_line_index )
{
    string compressed_line = "011"  + bitset< 5 >( start_location ).to_string() + bitset< 3 >( dic_line_index ).to_string();
    return compressed_line;
}/*double_con_bit_mismatch_comp_format()*/


/*!
 * @brief Encode double non - consective bit mismatches
 * @param start_location1 : location of the left mismatch(count from left)
 * @param start_location2 : location of the right mismatch(count from left)
 * @param dic_line_index : index of the dictionary entry
 * @return compressed data
 * ###return format
 | 100 | 1st ML from left  (5 bits) | 2nd ML from left  (5 bits) | Dic. index (3 bits) |
 |-----|:--------------------------:|:--------------------------:|:-------------------:|
 */
string double_any_bit_mismatch_comp_format( uint8_t start_location1, uint8_t start_location2 , uint8_t dic_line_index )
{
    string compressed_line = "100"  + bitset< 5 >( start_location1 ).to_string()+ bitset< 5 >( start_location2 ).to_string() + bitset< 3 >( dic_line_index ).to_string();
    return compressed_line;
}/*double_any_bit_mismatch_comp_format()*/


/*!
 * @brief Encode using direct matching format
 * @param dict_line_index : dictionary line index
 * @return compressed data
 * ###return format
 | 101 | Dic. index (3 bits) |
 |-----|:-------------------:|
 */
string dictionary_matching_comp_format( uint8_t dic_line_index  )
{
    string compressed_line = "101"  + bitset< 3 >( dic_line_index ).to_string(); // string conversion
    return compressed_line;
}/*dictionary_matching_comp_format()*/


/*!
 * @brief No valid encoding
 * @param data_line : input 32bit data
 * @return compressed data
 * ###return format
 | 110 | Original binary (32 bit) |
 |-----|:------------------------:|
 */
string no_comp_format(string data_line)
{
    string compressed_line = "110"  + data_line;
    return compressed_line;
}/*no_comp_format()*/






/******************Hamming calculations*******************
********************************************************/


/*!
 * @brief calulcate hamming distance and store mismatch positions
 * @param data_line : input 32bit data line
 * @param dic_line : dictionary data line
 * @param mismatch_indexes : store left most first 4 mismatch bit locations(LMSB = 0)
 * @return hamming distance
 * @note if hamming distance >4 then this functions returns always 5
 */
uint8_t hamming_dis_cal(string data_line ,string dic_line, uint8_t mismatch_indexes[])
{
    uint8_t hamming_dis=0;
    uint8_t index = 0;

    //loop through bits from left side and compare data_line with dictionary line
    for(uint8_t bit =0; bit<32; bit++)
    {
        if(data_line[bit] != dic_line[bit])
        {
            hamming_dis++;

            //NOTE: index of LMSB is 0
            mismatch_indexes[index] = bit;
            index++;
        }

        //No point of calculate if hamming distance is greater than 4
        //so we stop looping and storing mismatches.
        if(hamming_dis> MAX_MISMATCHES_RESOLVE)
        {
            break;
        }


    }

    return hamming_dis;
}/*hamming_dis_cal()*/

/*!
 * @brief decide that given compression technique can used with input.
 * @param format_to_check : input compression technique code (001 - 110)(no RLE code)
 * @param hamming_dis : hamming distance
 * @param mismatch_indexes : store left most first 4 mismatch bit locations(LMSB = 0)
 * @return format can be used or not
 * @warning We didn't consider first compression format since it needs additional data to figure out
 * it is possible or not to use it.
 */
bool format_check(uint8_t format_to_check , uint8_t hamming_dis , uint8_t mismatch_indexes[])
{
    bool method_validity = false;

    switch(format_to_check)
    {
    case 0b101 : //check for Direct matching
        if(hamming_dis == 0)
        {
            method_validity = true;
        }
        break;
    case 0b010 : //check for one bit mismatch
        if(hamming_dis == 1)
        {
            method_validity = true;
        }
        break;
    case 0b011 : //check for two consecutive bits mismatch
        if((hamming_dis == 2) & ((mismatch_indexes[1] - mismatch_indexes[0])==1) )
        {
            method_validity = true;
        }
        break;
    case 0b100 : //check for two non consecutive bits mismatch
        if((hamming_dis == 2) & ((mismatch_indexes[1] - mismatch_indexes[0])>1) )
        {
            method_validity = true;
        }
        break;
    case 0b001 : //check for bitmask-based compression
        if(((hamming_dis == 2) & ((mismatch_indexes[1] - mismatch_indexes[0])<BITMASK_SIZE)) |
           ((hamming_dis == 3) & ((mismatch_indexes[2] - mismatch_indexes[0])<BITMASK_SIZE)) |
           ((hamming_dis == 4) & ((mismatch_indexes[3] - mismatch_indexes[0])<BITMASK_SIZE)) )
        {
           method_validity = true;
        }
        break;
    case 0b110 : //no compression
        if(hamming_dis >4)
        {
            method_validity = true;
        }
        break;
    }
return method_validity;
}/*format_check()*/


/*!
 * @brief decide the RLE compression can be use or not
 * @param input_text_data : input 32 bit text data string array
 * @param index : comparing index
 * @param size_text_data : number of 32 bit data in the input array
 * @param p_num_repeats : pointer number of repeats(excluding first)
 * @return RLE can be used or not
 * ###return values
 * 0 - Cant RLE used .
 * 1 - We can use RLE. But we don't used here.So we neglect to use any format.
 * 2 - We can use RLE here . In this case we consider number of repetitions.
 * @note :We always use RLE only after first repetition.
 */
uint8_t rle_format_check(string input_text_data[] , uint8_t index, uint8_t size_text_data , uint8_t *p_num_repeats)
{
    uint8_t lower_count=0;
    uint8_t upper_count=0;

    uint8_t rle_decision = RLE_CANT_USE;
    //0 - cant used
    //1 - not used
    //2 - can used / number of repetitions are consider here.

    //reverse count
    for(uint8_t lower_index = index; lower_index>0; lower_index-- )
    {
        if(input_text_data[index] == input_text_data[lower_index - 1])
        {
            lower_count++;
        }
        else
        {
            break;
        }
    }


    //forward count
    for(uint8_t upper_index = index; upper_index<size_text_data - 1 ; upper_index++ )
    {

        if(input_text_data[index] == input_text_data[upper_index + 1])
        {
            upper_count++;
        }
        else
        {
            break;
        }
    }

    if(lower_count == 0)
    {
        rle_decision = RLE_CANT_USE; //RLE cant used
    }
    else if(lower_count == 1)
    {
        rle_decision = RLE_USE; //RLE can used
        *p_num_repeats = lower_count+upper_count;
    }
    else
    {
        rle_decision = RLE_NOT_USE; //RLE can used .but we not used
    }

  return  rle_decision;

}/*rle_format_check()*/


/*!
 * @brief choose best method for given input from last 6 methods(excluding RLE)
 * @param data_line : input data string 32 bit line
 * @param hamming_distances : array of hamming distances corresponding to the all dictionary entries.
 * @param all_mismatch_indexes : store left most first 4 mismatch bit locations(LMSB = 0) corresponding to the dictionary entries.
 * @param selection_details : store method index and its corresponding dictionary index.
 * @return non
 * @note selection_details[0] =>selected method index
 * selection_details[1] =>corresponding dic. index
 * @note We didn't consider first compression format since it needs additional data to figure out
 * it is possible or not to use it.
 */
void best_format(string data_line , uint8_t hamming_distances[] ,uint8_t all_mismatch_indexes[][5] ,uint8_t selection_details[])
{



    //methods ordered in a most profitable way. (exclude 0b000 method.)
    uint8_t method_priorities[6] = {0b101 , 0b010 , 0b011 , 0b001 , 0b100 , 0b110};


    bool method_found =false;

    //loop until best method found.
    for (uint8_t priority_index =0 ;priority_index< 6; priority_index++)
    {
        for(uint8_t dic_index =0 ;dic_index< DIC_SIZE; dic_index++)
        {
           if(format_check(method_priorities[priority_index] ,hamming_distances[dic_index], all_mismatch_indexes[dic_index] ) == true)
           {
               method_found = true;

               //save method index and corresponding dicionary index
               selection_details[0] = method_priorities[priority_index];
               selection_details[1] = dic_index;

               break;
           }
        }

        //if method is found no point to loop.
        if(method_found == true)
        {
            break;
        }
    }

}/* best_format()*/

/*!
 * @brief generate bitmask.
 * @param hamming_dis : hamming distance
 * @param mismatch_indexes : store left most first 4 mismatch bit locations(LMSB = 0).
 * @return bitmask string (4 bits)
 * @note :in bitmask method start index is always left most first mismatch.
*/
string bit_mask_genearate(uint8_t hamming_dis , uint8_t mismatch_indexes[] )
{

    string bit_mask="";

    switch(hamming_dis)
    {
    case 2://2 bit mismatch
        if((mismatch_indexes[1] - mismatch_indexes[0]) == 3 ) // (X00X)
        {
         bit_mask = "1001";
        }
        else //(0X0X ,X0X0)
        {
            if(mismatch_indexes[0]==29) //last three bits
            {
                bit_mask = "0101";
            }
            else
            {
               bit_mask = "1010" ;
            }

        }

        break;
    case 3://3 bit mismatch
        if((mismatch_indexes[1] - mismatch_indexes[0]) == 2 ) // (X0XX)
        {
            bit_mask = "1011";
        }
        else if((mismatch_indexes[2] - mismatch_indexes[1]) == 2 ) // (XX0X)
        {
            bit_mask = "1101";
        }
        else //(XXX0 ,0XXX))
        {
            if(mismatch_indexes[0]==29)
            {
                bit_mask = "0111";
            }
            else
            {
               bit_mask = "1110" ;
            }

        }
        break;
    case 4://4 bit mismatch (XXXX)
        bit_mask = "1111";
        break;
    }

    return bit_mask;
}/* bit_mask_genearate()*/

/*!
 * @brief compress single 32 bit data
 * @param input_text_data[] : input 32 bit data array
 * @param dictionary : dictionary array
 * @param data_line_index : index of the data line.
 * @param size_input_data : number of 32 bit data
 * @return compressed data in string format.
 */
string compress_data_line(string input_text_data[] , string dictionary[] , uint8_t data_line_index , uint8_t size_input_data )
{
    uint8_t num_repeats = 0;
    uint8_t rle_result = rle_format_check(input_text_data ,data_line_index, size_input_data, &num_repeats);
    string compressed_line = "";

    if (rle_result == RLE_USE) //use RLE
    {
        //RLE Format
        compressed_line = rle_comp_format(num_repeats);

    }
    else if(rle_result == RLE_CANT_USE) // cant use RLE
    {
        //other 6 methods
        uint8_t hamming_distances[DIC_SIZE]={};
        uint8_t all_mismatch_indexes[DIC_SIZE][5] = {{}};

        //calculate and store hamming distances for all dictionary entries.
        for(uint8_t index =0 ;index< DIC_SIZE; index++)
        {
            hamming_distances[index] = hamming_dis_cal(input_text_data[data_line_index] , dictionary[index] ,all_mismatch_indexes[index] );
        }

        uint8_t selection_details[2]={};
        best_format(input_text_data[data_line_index], hamming_distances , all_mismatch_indexes , selection_details );

        switch (selection_details[0])
        {
            case 0b001: //bit mask comparession
                {
                string bit_mask = bit_mask_genearate(hamming_distances[selection_details[1]] ,all_mismatch_indexes[selection_details[1]] );
                uint8_t starting_index = (all_mismatch_indexes[selection_details[1]][0] >= 29) ? 28 : all_mismatch_indexes[selection_details[1]][0];
                compressed_line = bit_mask_comp_format(starting_index  ,bit_mask, selection_details[1]);
                break;
                }
            case 0b010: //1 bit mismatch
                compressed_line = single_bit_mismatch_comp_format(all_mismatch_indexes[selection_details[1]][0] ,selection_details[1] );
                break;
            case 0b011: //2 bit consecutive mismatch
                compressed_line = double_con_bit_mismatch_comp_format(all_mismatch_indexes[selection_details[1]][0] ,selection_details[1] );
                break;
            case 0b100: //2 bit non consecutive mismatch
                compressed_line= double_any_bit_mismatch_comp_format(all_mismatch_indexes[selection_details[1]][0] , all_mismatch_indexes[selection_details[1]][1] ,selection_details[1]);
                break;
            case 0b101: // dictionary matching
                compressed_line = dictionary_matching_comp_format(selection_details[1]);
                break;
            case 0b110: //no compression
                compressed_line = no_comp_format(input_text_data[data_line_index]);
                break;
        }
    }
    //if rle_result is 1 then we neglect

return compressed_line;

}/*compress_data_line()*/


/*!
 * @brief generate dictionary and compression all the data
 *
 * ###Example code
 * @code
 *   compressing(input_file,input_text_data,dictionary, output_data);
 * @endcode
 *
 * @param input_file : input file name
 * @param input_text_data data : 32bit string
 * @param dictionary : dictionary array
 * @param output_data : String array.
 * @param p_input_size : pointer to input file  size
 * @return compressed data array size
 * @note input_text_data , dictionary , output_data , p_input_size are empty variables which we want to store data.
 */
uint8_t compressing(string input_file_name , string input_text_data[] ,string dictionary[] ,string output_data[] , uint8_t *p_input_size)
{

    uint8_t data_size = read_text(input_file_name,input_text_data );
    *p_input_size = data_size;
    uint8_t output_index = 0;

    build_dictionary(data_size,input_text_data, dictionary );

    string compressed_data_line="";
    string compressed_32_bit_data_line="";

    for(uint8_t data_line_index = 0; data_line_index < data_size; data_line_index++)
    {
        compressed_data_line = compress_data_line(input_text_data , dictionary , data_line_index , data_size);
        uint8_t new_len = compressed_data_line.length() + compressed_32_bit_data_line.length();
        uint8_t pre_len = compressed_32_bit_data_line.length();

        //make 32 bit data line
        if(new_len<32) //pre length + new length <32 - >update line
        {
            //update line
            compressed_32_bit_data_line+=compressed_data_line;

        }
        else if(new_len==32) //pre length + new length  = 32 - >reset line
        {
            //store 32 bit line
            compressed_32_bit_data_line +=compressed_data_line;
            output_data[output_index]= compressed_32_bit_data_line;
            output_index++;
            //reset
            compressed_32_bit_data_line = "";
        }
        else if((new_len>32) & (new_len <64))//pre length + new length  > 32  and <64
        {
            //complete existing line and store it
            compressed_32_bit_data_line += compressed_data_line.substr(0 , 32 - pre_len);
            output_data[output_index] = compressed_32_bit_data_line;
            output_index++;

            //update new line
            compressed_32_bit_data_line = compressed_data_line.substr(32 - pre_len , compressed_data_line.length() - (32 - pre_len) );
        }

        else if(new_len == 64)//pre length + new length  = 4
        {
            //complete existing line and store it
            compressed_32_bit_data_line += compressed_data_line.substr(0 , 32 - pre_len);
            output_data[output_index] = compressed_32_bit_data_line;
            output_index++;

            //complete new line and store it
            compressed_32_bit_data_line = compressed_data_line.substr(32 - pre_len , compressed_data_line.length() - (32 - pre_len) );
            output_data[output_index] = compressed_32_bit_data_line;
            output_index++;

            //update new line
            compressed_32_bit_data_line = "";
        }

        else if(new_len > 64)//pre length + new length  = 4
        {
            //complete existing line and store it
            compressed_32_bit_data_line += compressed_data_line.substr(0 , 32 - pre_len);
            output_data[output_index] = compressed_32_bit_data_line;
            output_index++;

            //complete new line and store it
            compressed_32_bit_data_line = compressed_data_line.substr(32 - pre_len , 32 );
            output_data[output_index] = compressed_32_bit_data_line;
            output_index++;


            //update new line
            compressed_32_bit_data_line = compressed_data_line.substr(64- pre_len , compressed_data_line.length() -(64- pre_len ) );

        }

        if(data_line_index == (data_size -1) )//after compressing final data
        {
            string padding = "11111111111111111111111111111111";
            if(compressed_32_bit_data_line.length() != 0) //if line is fully filled then size is 0
            {
                compressed_32_bit_data_line += padding.substr(0, 32 - compressed_32_bit_data_line.length() );
                output_data[output_index] = compressed_32_bit_data_line;
                output_index++;

            }
        }
    }


    //save compressed data
    // 1 . build array
    output_data[output_index] = "xxxx";
    for(uint8_t index = 0;index<DIC_SIZE;index++)
    {
       output_data[index + output_index + 1 ] = dictionary[index];
    }

    //write data
    write_text("cout.txt" ,output_data,output_index + 1 + DIC_SIZE);


    return output_index;
}/*compressing()*/


/******************************************************************************************************************************
***************************************************** decompressor ************************************************************
*******************************************************************************************************************************/

/*!
 * @brief decompress already compressed data
 *
 * ###Example code
 * @code
 *   decompressing("compressed.txt", compressed_data ,dictionary,output_decompressed);
 * @endcode
 *
 * @param compressed_file_name : name of the text file where includes compressed data
 * @param dictionary : dictionary array
 * @param output_decompressed : array of strings where stored decoded 32 bit numbers as strings.
 * @return number of 32 bit data lines in original file.
 */
uint8_t decompressing(string compressed_file_name , string dictionary[] , string output_decompressed[])
{
    string compressed_data[128]={};
    string text_data[138]={};


    read_text(compressed_file_name, text_data);

    //extract dictionary and compressed data from text_data
    uint8_t compressed_size = 0;
    while (text_data[compressed_size] != "xxxx")
    {
        compressed_data[compressed_size] = text_data[compressed_size];
        compressed_size++;
    }


    for(uint8_t index = 0; index<DIC_SIZE; index++)
    {
        dictionary[index] = text_data[index + compressed_size + 1];
    }



    //we always consider current line with two next lines.that will store in the searching line.
    string searching_line= "";
    string compression_id = "";
    string compressed_line = "" ;

    uint16_t executed_len = 0; //number of bits already executed from the start.
    uint16_t bit_index = 0; //index of the current bit respect to existing line
    uint8_t decompressed_index = 0;
    //loop through all the bits

    while(executed_len < compressed_size*32)
    {
        //NOTE : bit index starting from 0.
        if((executed_len/32) == (compressed_size - 1)) //before last line
        {
            searching_line = compressed_data[(executed_len/32)];
        }
        else if((executed_len/32) == (compressed_size - 2)) //before last 2 lines
        {
            searching_line = compressed_data[(executed_len/32)] + compressed_data[(executed_len/32) + 1];
        }
        else
        {
            searching_line = compressed_data[(executed_len/32)] + compressed_data[(executed_len/32) + 1] + compressed_data[(executed_len/32) + 2];
        }

        if((bit_index>=32) & (bit_index<64))
        {
            bit_index = bit_index-32;
        }
        else if(bit_index>=64)
        {
            bit_index = bit_index-64;
        }

        //extract compression ID
        compression_id = searching_line.substr(bit_index,3);


        //extract compressed data
        if(compression_id == "000") //bit mask(12 bits)
        {
            compressed_line = "000" + searching_line.substr(bit_index + 3 ,2);
            //update bit_index
            bit_index += 5;

            //decompress data
            decompressed_index = rle_dec(compressed_line , output_decompressed , decompressed_index);
        }
        else if(compression_id == "001") //bit mask(12 bits)
        {

            compressed_line = "001" + searching_line.substr(bit_index + 3 ,12);
            //update bit_index
            bit_index += 15;

            //decompress data
            decompressed_index = bit_mask_dec(compressed_line , dictionary ,output_decompressed , decompressed_index );
        }
        else if(compression_id == "010") //single bit mismatch(8 bits)
        {

            compressed_line = "010" + searching_line.substr(bit_index + 3 ,8);
            //update bit_index
            bit_index += 11;

            //decompress data
            decompressed_index = single_bit_mismatch_dec(compressed_line , dictionary ,output_decompressed , decompressed_index );
        }
        else if(compression_id == "011") //double con. bit mismatch(8 bits)
        {

            compressed_line = "011" + searching_line.substr(bit_index + 3 ,8);
            //update bit_index
            bit_index += 11;

            //decompress data
            decompressed_index = double_con_bit_mismatch_dec(compressed_line , dictionary ,output_decompressed , decompressed_index );
        }
        else if(compression_id == "100") //double non con. bit mismatch(13 bits)
        {

            compressed_line = "100" + searching_line.substr(bit_index + 3 ,13);
            //update bit_index
            bit_index += 16;

            //decompress data
            decompressed_index = double_any_bit_mismatch_dec(compressed_line , dictionary ,output_decompressed , decompressed_index );
        }
        else if(compression_id == "101") //dictionary match (3 bits)
        {

            compressed_line = "101" + searching_line.substr(bit_index + 3 ,3);

            //update bit_index
            bit_index += 6;

            //decompress data
            decompressed_index = direct_match_dec(compressed_line , dictionary ,output_decompressed , decompressed_index );
        }
        else if(compression_id == "110") //uncompressed data(32 bit)
        {

            compressed_line = "110" + searching_line.substr(bit_index + 3 ,32);

            //update bit_index
            bit_index += 35;

            //decompress data
            decompressed_index = no_compression_dec(compressed_line , output_decompressed , decompressed_index);
        }
        else
        {
            break;

        }

        executed_len += compressed_line.length();
    }

    //save decompressed data
    write_text("dout.txt" ,output_decompressed,decompressed_index);

    return decompressed_index;
}/*decompressing()*/

/*!
 * @brief decompress direct match
 * @param compressed_line : compressed data string
 * @param dictionary : dictionary array
 * @param output_decompressed : array of strings where stored decoded 32 bit numbers as strings.
 * @param index : index which used to store in output_decompressed.
 * @return next index to store .
 */
uint8_t direct_match_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index )
{
    uint8_t dic_index = str_to_val(compressed_line.substr(3,3));

    //store decompressed data
    output_decompressed[index] = dictionary[dic_index];

    return index+1;
}/*direct_match_dec()*/

/*!
 * @brief decompress single mismatch
 * @param compressed_line : compressed data string
 * @param dictionary : dictionary array
 * @param output_decompressed : array of strings where stored decoded 32 bit numbers as strings.
 * @param index : index which used to store in output_decompressed.
 * @return next index to store .
 */
uint8_t single_bit_mismatch_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index )
{
    //data extract
    uint8_t bit_index = str_to_val(compressed_line.substr(3,5));
    uint8_t dic_index = str_to_val(compressed_line.substr(8,3));

    //reverse calculation
    string dic_line = dictionary[dic_index];
    uint8_t mismatch_bit = char_to_bit(dic_line[bit_index]); //1st bit
    char corrected_bit =  bit_to_char( 1 - mismatch_bit ); //1st bit
    dic_line[bit_index] = corrected_bit; //1st bit

    //store decompressed data
    output_decompressed[index] = dic_line;

    return index+1;
} /*single_bit_mismatch_dec()*/

/*!
 * @brief decompress consecutive double mismatches
 * @param compressed_line : compressed data string
 * @param dictionary : dictionary array
 * @param output_decompressed : array of strings where stored decoded 32 bit numbers as strings.
 * @param index : index which used to store in output_decompressed.
 * @return next index to store .
 */
uint8_t double_con_bit_mismatch_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index )
{
    //data extract
    uint8_t bit_index = str_to_val(compressed_line.substr(3,5));
    uint8_t dic_index = str_to_val(compressed_line.substr(8,3));

    //reverse calculation
    string dic_line = dictionary[dic_index];
    uint8_t mismatch_bit1 = char_to_bit(dic_line[bit_index]) ;  //1st bit
    uint8_t mismatch_bit2 = char_to_bit(dic_line[bit_index+1]); //2nd bit
    char corrected_bit1 = bit_to_char( 1 - mismatch_bit1 ); //1st bit
    char corrected_bit2 = bit_to_char( 1 - mismatch_bit2 ); //2nd bit
    dic_line[bit_index] = corrected_bit1;   //1st bit
    dic_line[bit_index+1] = corrected_bit2; //2nd bit

    //store decompressed data
    output_decompressed[index] = dic_line;

    return index+1;
}/*double_con_bit_mismatch_dec()*/

/*!
 * @brief decompress non consecutive double mismatches
 * @param compressed_line : compressed data string
 * @param dictionary : dictionary array
 * @param output_decompressed : array of strings where stored decoded 32 bit numbers as strings.
 * @param index : index which used to store in output_decompressed.
 * @return next index to store .
 */
uint8_t double_any_bit_mismatch_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index )
{
    //data extract
    uint8_t bit_index1 = str_to_val(compressed_line.substr(3,5));
    uint8_t bit_index2 = str_to_val(compressed_line.substr(8,5));
    uint8_t dic_index = str_to_val(compressed_line.substr(13,3));

    //reverse calculation
    string dic_line = dictionary[dic_index];
    uint8_t mismatch_bit1 = char_to_bit(dic_line[bit_index1]) ; //1st bit
    uint8_t mismatch_bit2 = char_to_bit(dic_line[bit_index2]) ; //2nd bit
    char corrected_bit1 = bit_to_char( 1 - mismatch_bit1 ); //1st bit
    char corrected_bit2 = bit_to_char( 1 - mismatch_bit2 ); //2nd bit
    dic_line[bit_index1] = corrected_bit1; //1st bit
    dic_line[bit_index2] = corrected_bit2; //2nd bit

    //store decompressed data
    output_decompressed[index] = dic_line;

    return index+1;
}/*double_any_bit_mismatch_dec()*/

/*!
 * @brief decompress the bitmask compressed data
 * @param compressed_line : compressed data string
 * @param dictionary : dictionary array
 * @param output_decompressed : array of strings where stored decoded 32 bit numbers as strings.
 * @param index : index which used to store in output_decompressed.
 * @return next index to store .
 */
uint8_t bit_mask_dec(string compressed_line , string dictionary[] ,string output_decompressed[] , uint8_t index )
{
    //data extract
    uint8_t bit_index = str_to_val(compressed_line.substr(3,5));
    string bitmask =  compressed_line.substr(8,4);
    uint8_t dic_index = str_to_val(compressed_line.substr(12,3));

    //reverse calculation
    string dic_line = dictionary[dic_index];
    uint8_t mask_bit1 = char_to_bit(bitmask[0]) ; //1st bit
    uint8_t mask_bit2 = char_to_bit(bitmask[1]) ; //2nd bit
    uint8_t mask_bit3 = char_to_bit(bitmask[2]) ; //3rd bit
    uint8_t mask_bit4 = char_to_bit(bitmask[3]) ; //4th bit
    uint8_t mismatch_bit1 = char_to_bit(dic_line[bit_index]) ;  //1st bit
    uint8_t mismatch_bit2 = char_to_bit(dic_line[bit_index+1]); //2nd bit
    uint8_t mismatch_bit3 = char_to_bit(dic_line[bit_index+2]); //3rd bit
    uint8_t mismatch_bit4 = char_to_bit(dic_line[bit_index+3]); //4th bit
    char corrected_bit1 = bit_to_char(mask_bit1^mismatch_bit1); //1st bit
    char corrected_bit2 = bit_to_char(mask_bit2^mismatch_bit2); //2nd bit
    char corrected_bit3 = bit_to_char(mask_bit3^mismatch_bit3); //3rd bit
    char corrected_bit4 = bit_to_char(mask_bit4^mismatch_bit4); //4th bit

    dic_line[bit_index] = corrected_bit1;   //1st bit
    dic_line[bit_index+1] = corrected_bit2; //2nd bit
    dic_line[bit_index+2] = corrected_bit3; //3rd bit
    dic_line[bit_index+3] = corrected_bit4; //4th bit

    //store decompressed data
    output_decompressed[index] = dic_line;

    return index+1;
}/*bit_mask_dec()*/

/*!
 * @brief decompress the non compressed code
 * @param compressed_line : compressed data string
 * @param output_decompressed : array of strings where stored decoded 32 bit numbers as strings.
 * @param index : index which used to store in output_decompressed.
 * @return next index to store .
 */
uint8_t no_compression_dec(string compressed_line , string output_decompressed[] , uint8_t index )
{
    //store decompressed data
    output_decompressed[index] = compressed_line.substr(3, 32);

    return index+1;
}/*no_compression_dec()*/

/*!
 * @brief decompress the RLE compressed code
 * @param compressed_line : compressed data string
 * @param output_decompressed : array of strings where stored decoded 32 bit numbers as strings.
 * @param index : index which used to store in output_decompressed.
 * @return next index to store .
 */
uint8_t rle_dec(string compressed_line ,string output_decompressed[] , uint8_t index )
{
    uint8_t repetition = str_to_val(compressed_line.substr(3,2)) + 1;

    //store decompressed data
    for(uint8_t line = 0; line<repetition; line++)
    {
       output_decompressed[index + line] = output_decompressed[index-1];
    }

    return index+repetition;
}/*rle_dec()*/

/*!
 * @brief convert string to number
 * ###Example code
 * @code
 *   str_to_val("1011") //"1011"-> 0b1011
 * @endcode
 * @param str : number as string (e.g.:"1011")
 * @return binary number
 * @warning : input string should have 2 or more characters.
 * else use char_to_val() function.
 * @see : char_to_bit()
 */
uint8_t str_to_val(string str )
{
    uint8_t val =0;
    for(uint8_t bit =0;bit<str.length(); bit++)
    {
        val += (char_to_bit(str[bit])*pow(2, str.length() - bit - 1));
    }

    return val;
}/*str_to_val()*/

/*!
 * @brief convert character bit to numerical
 * ###Example code
 * @code
 *   char_to_bit('1') //'1' -> 1
 * @endcode
 * @param bit : char ('1','0')
 * @return number (1,0)
 */
uint8_t char_to_bit(char bit)
{
    return (bit== '1') ? 1:0;
}/*char_to_bit()*/

/*!
 * @brief convert numerical bit to character
 * ###Example code
 * @code
 *   bit_to_char(1) //1 -> '1'
 * @endcode
 * @param bit : number (1,0)
 * @return character ('1' ,'0')
 */
char bit_to_char(uint8_t bit)
{
    return 48 + bit;
}/*bit_to_char()*/
