//Dan Mallerdino
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <bits/stdc++.h>
#include <gmp.h>
#include <cstdlib>
#include<ctime>
using namespace std;

void Decrypt();
void Encrypt();
void Keygen();
void Factor();

long p = 0;
long q = 0;

int main()
{
    char Answer;
    cout << "What do you wish to do? Decrypt, Encrypt, or generate a Key (D / E / K)" << endl;
    cin >> Answer;
    if(Answer == 'D')
    Decrypt();
    if(Answer == 'E')
    Encrypt();
    if(Answer == 'K')
    Keygen();

        return 0;
}

void Keygen()
{
    srand((unsigned)time(0)); 
    string answer;
    cout <<"Public or Private key? (Pub / Priv)" << endl;
    cin >> answer;
    if(answer == "Pub")
    {
        string filename;
        cout << "What is the name of the file containing the private key?" << endl;
        cin >> filename;
        ifstream privfileread;
        privfileread.open(filename.c_str());
        string N;
        string d;
            int dlength = d.length();
            char d_array[dlength+1];
            
        string p;
            int plength = p.length();
            char p_array[plength+1];
            
        string q;
            int qlength = q.length();
            char q_array[qlength+1];
        
        privfileread >> N >> d >> p >> q;
            strcpy(q_array, q.c_str());
            strcpy(p_array, p.c_str());
            strcpy(d_array, d.c_str());
        privfileread.close();
        cout << N << " " << d << " " << p << " " << q << endl;
        //calculating e

            mpz_t dval;
            mpz_init(dval);
            mpz_set_str (dval, d_array, 36);

            mpz_t qval;
            mpz_init(qval);
            mpz_set_str (qval, q_array, 36);
                mpz_t phiq;
                mpz_init(phiq);
                mpz_sub_ui(phiq,qval,1);
                

            mpz_t pval;
            mpz_init(pval);
            mpz_set_str (pval, p_array, 36);
                mpz_t phip;
                mpz_init(phip);
                mpz_sub_ui(phip,pval,1);

            mpz_t Phi;
            mpz_init(Phi);
            mpz_mul(Phi,phip,phiq);

            mpz_t eval;
            mpz_init(eval);
            mpz_invert(eval,dval,Phi);
            
                int elength=mpz_sizeinbase(eval,36);
                char e_array[elength+1];
                mpz_get_str(e_array,36,eval);

            for(int i = 0; i < elength; i++)
                cout << e_array[i];
            cout << endl;

            mpz_clear(eval);
            mpz_clear(dval);
            //mpz_clear(Phival);
        //done calculating e
        string pubkeyname;
        cout <<"What is the filename you would like to put the public key in?" << endl;
        cin >> pubkeyname;
        ofstream pubfilewrite;
        string e(e_array);
        pubfilewrite.open(pubkeyname.c_str());
        pubfilewrite << N << " " << e;
        pubfilewrite.close();
    }
    if(answer == "Priv")
    {
    
        //Getting the file name
            string Privkeyfile;
            cout << "What is the name of the file for your Private key to be stored in?" << endl;
            cin >> Privkeyfile;

        //getting information to calculate P
            long lengthofP;
            cout <<"How long would you like p to be?" << endl;
            cin >> lengthofP;
            if(lengthofP >= 7)
            {
                cout <<"Please note that for any length greater than 6 the run time will increase by quite a bit." << endl;
                cout <<"Confirm your size: ";
                cin >> lengthofP;
            }
        //generating P
                mpz_t P;
                mpz_init(P);
            
                mpz_t minP;
                mpz_init(minP);
                
                mpz_t maxP;
                mpz_init(maxP);


                mpz_set_ui (minP, 10);
                mpz_pow_ui ( minP, minP, lengthofP-1);
                
                mpz_set_ui (maxP, 10);
                mpz_pow_ui ( maxP, maxP, lengthofP);
                mpz_sub_ui (maxP, maxP, 1);    
                   
                gmp_randstate_t state;
                gmp_randinit_default (state);
                
                mpz_set_ui (P, 10);

                while(mpz_probab_prime_p (P, 30) != 2)
                {   
                    mpz_urandomm(P, state, P);
                    mpz_add(P,P,minP);
                }

                gmp_randclear (state);
                long p = mpz_get_ui (P);
        //P generated

        //getting information to calculate q
            long lengthofQ;
            cout <<"How long would you like q to be?(should be around the same length as p)" << endl;
            cin >> lengthofQ;
        //generating Q
                mpz_t Q;
                mpz_init(Q);
            
                mpz_t minQ;
                mpz_init(minQ);
                
                mpz_t maxQ;
                mpz_init(maxQ);

                mpz_set_ui (minQ, 10);
                mpz_pow_ui ( minQ, minQ, lengthofQ-1);
                
                mpz_set_ui (maxQ, 10);
                mpz_pow_ui ( maxQ, maxQ, lengthofQ);
                mpz_sub_ui (maxQ, maxQ, 1);    

                gmp_randinit_default (state);   

                mpz_set_ui (Q, 10);

                while((mpz_probab_prime_p (Q, 30) != 2)&&(mpz_get_ui(Q)!=mpz_get_ui(P)))
                {                
                    mpz_urandomm(Q, state, Q);
                    mpz_add(Q,Q,minQ);
                }
                gmp_randclear (state);
                long q = mpz_get_ui (Q);
                    
        //Q generated

        //calculating N based on p and q
            mpz_t N;
            mpz_init(N);
            mpz_mul(N,P,Q);
            long n = mpz_get_ui (N);
            cout << "N = " << n << endl;
            cout <<"p = " << p << endl;
            cout << "q = " << q << endl;

        //calculating D
            mpz_t phip;
            mpz_init(phip);
            mpz_sub_ui(phip,P,1);// phip = P-1

            mpz_t phiq;
            mpz_init(phiq);
            mpz_sub_ui(phiq,Q,1);//phiq = Q -1

            mpz_t Phi;
            mpz_init(Phi);
            mpz_mul(Phi,phiq,phip); // Phi(N) = phip*phiq

            mpz_t D;
            mpz_init(D); // initializing D

            mpz_t E;
            mpz_init(E); 
            mpz_set_ui(E,65537);// initializing E = 65537

           // mpz_cdiv_q (D, Phi, E); // if D*E == 1modPhi(N) = Phi(N)
            mpz_invert (D, E, Phi);

            long dget = mpz_get_ui (D);
            
            cout << "d = " << dget << endl;


        // converting N, d, p, q to base 36
            char  Nconv[mpz_sizeinbase (N, 36)+2];
        mpz_get_str (Nconv, 36, N);

            char  Pconv[mpz_sizeinbase (P, 36)+2];
        mpz_get_str (Pconv, 36, P);

            char  Qconv[mpz_sizeinbase (Q, 36)+2];
        mpz_get_str (Qconv, 36, Q);

            char  Dconv[mpz_sizeinbase (D, 36)+2];
        mpz_get_str (Dconv, 36, D);


        //File writing N, d, p, q
        ofstream privfilewrite;
        privfilewrite.open(Privkeyfile.c_str());

        
        for(int h = 0; h < mpz_sizeinbase(N,36); h++)
            privfilewrite << Nconv[h];
            privfilewrite << " ";

        for(int h = 0; h < mpz_sizeinbase(D,36); h++)
            privfilewrite << Dconv[h];
            privfilewrite << " ";

        for(int h = 0; h < mpz_sizeinbase(P,36); h++)
            privfilewrite << Pconv[h];
            privfilewrite << " ";

        for(int h = 0; h < mpz_sizeinbase(Q,36); h++)
            privfilewrite << Qconv[h];
            privfilewrite << endl;

        mpz_clear(P);
        mpz_clear(Q);
        mpz_clear(N);
        mpz_clear(D);
        mpz_clear(phip);
        mpz_clear(phiq);
        mpz_clear(Phi);

       privfilewrite.close();
    }
}

void Encrypt()
{
    cout <<"What is the name of your public key file?" << endl;
    string pubkeyfile;
    cin >> pubkeyfile;
    
    string Nbase;
    string Ebase;

    ifstream keyfileread;
        keyfileread.open(pubkeyfile.c_str());
        keyfileread >> Nbase >> Ebase;
        keyfileread.close();

        cout << Nbase << " " << Ebase << endl;
            int Nlength = Nbase.length();  
      
            // declaring character array 
            char N_array[Nlength+1];  
            
            // copying the contents of the  
            // string to char array 
            strcpy(N_array, Nbase.c_str()); 
                     
            mpz_t modulus;
            mpz_init(modulus);
            mpz_set_str (modulus, N_array, 36);

            int Elength = Ebase.length();  
      
            // declaring character array 
            char E_array[Elength+1];  
            
            // copying the contents of the  
            // string to char array 
            strcpy(E_array, Ebase.c_str()); 

            mpz_t expo;
            mpz_init(expo);
            mpz_set_str (expo, E_array, 36);


        string Encryptfile;
        cout << "Please enter the filename with the text to be encrypted." << endl;
        cin >> Encryptfile;

        string Encryptoutput;
        cout <<"Please enter the filename with the output: " << endl;
        cin >> Encryptoutput;

    ifstream textfileread;
    ofstream textfilewriter;
        textfileread.open(Encryptfile.c_str());
        textfilewriter.open(Encryptoutput.c_str());
        char ptchar;
        cout <<"{Plaintext word -> Encrypted word}" << endl;
        while(!textfileread.eof())
        {
            string ptwordtoint = "";

            long currentletter;
            string ptword = "";
            
           for(int x = 0; x < Nlength-1; x++)
           {
               if(textfileread.eof())
               {
                   break;
               }
               textfileread >> ptchar;
               if( ((int)ptchar < (int)'A')|| ((int)ptchar > (int)'z'))
                {
                    x--;
                    continue;
                }
               ptword += ptchar;
           }

            
            cout <<"{ " << ptword << " -> ";
            // declaring character array 
            char char_array[7];  
            // copying the contents of the  
            // string to char array 
            strcpy(char_array, ptword.c_str()); 
            
            //Declaring the mpz_t ROP
            mpz_t rop; 
            mpz_init(rop);
            //setting rop = the first 6 characters in the text file base 36
            mpz_set_str (rop, char_array, 36);
            //rasing rop to the exponent expo mod modulus
            mpz_t encryptedword;
            mpz_init(encryptedword);
            mpz_powm_sec(encryptedword, rop, expo, modulus);

            //getting the length of rop base 36
            int length = mpz_sizeinbase (encryptedword, 36);
            //getting rop into a base 36 char array
            char checkword[length+1];
            mpz_get_str(checkword,36,encryptedword);
            
            for(int i = 0; i < length; i++)
            {
                cout << checkword[i];
                textfilewriter << checkword[i]; 
            }
            textfilewriter << " ";
            cout << "}" << endl;
            
            mpz_clear(rop);
            //mpz_clear(expo);
            //mpz_clear(modulus);
            mpz_clear(encryptedword);
        }

        textfileread.close();
        textfilewriter.close();

        return;
}

void Decrypt()
{
    cout <<"What is the name of your private key file?" << endl;
    string privkeyfile;
    cin >> privkeyfile;

    ifstream privkeyfilereader;
        privkeyfilereader.open(privkeyfile.c_str());
        //N D P Q
        string Nbase, Dbase, Pbase, Qbase;

        privkeyfilereader >> Nbase >> Dbase >> Pbase >> Qbase;

        cout << "N base 36 = " << Nbase << endl;
        cout << "D base 36 = " <<  Dbase << endl;
        cout << "P base 36 = " <<  Pbase << endl;
        cout << "Q base 36 = " <<  Qbase << endl;
        
        int Nlength = Nbase.length();  
      
            // declaring character array 
            char N_array[Nlength+1];  
            
            // copying the contents of the  
            // string to char array 
            strcpy(N_array, Nbase.c_str()); 
                       
            mpz_t modulus;
            mpz_init(modulus);
            mpz_set_str(modulus, N_array, 36);

        int Dlength = Dbase.length();  
      
            // declaring character array 
            char D_array[Dlength+1];  
            
            // copying the contents of the  
            // string to char array 
            strcpy(D_array, Dbase.c_str()); 
                     
            mpz_t expo;
            mpz_init(expo);
            mpz_set_str (expo, D_array, 36);

        string readcipher;
        cout <<"Please enter the name of the file you would like to read from: " << endl;
        cin >> readcipher;

        string writept;
        cout <<"Please enter the name of the file you would like to write to: " << endl;
        cin >> writept;

        ifstream decryptcipher;
        ofstream writeplaintext;
        decryptcipher.open(readcipher.c_str());
        writeplaintext.open(writept.c_str());
        while(!decryptcipher.eof())
        {
            string cipherword;
            mpz_t word;
            mpz_init(word);
            decryptcipher >> cipherword;

            int length = cipherword.length();  
      
            // declaring character array 
            char char_array[length+1];  
            
            // copying the contents of the  
            // string to char array 
            strcpy(char_array, cipherword.c_str()); 
  
            mpz_set_str (word, char_array, 36);
            mpz_t decryptedword;
            mpz_init(decryptedword);
            mpz_powm_sec(decryptedword, word, expo, modulus);

            int wordlength = mpz_sizeinbase (decryptedword, 36);
            char word_array[wordlength+1];
            mpz_get_str(word_array, 36, decryptedword);
            string decryptword(word_array);
            cout << "WORD: " << decryptword << endl;
            writeplaintext << decryptword;

            cout << endl;
        }

    return;
}