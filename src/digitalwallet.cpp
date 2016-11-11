const char HELP[] = "\n      *********************************************************************************\n\
        Insight Data Program: Data Engineering Challenge. `Digital Wallet: Fraud Alert'\n\
        By: Mohamad Nasr-Azadani, Email: mmnasr@gmail.com\n\n\
        <./EXEC_FILE>   <BATCH-PAYMENT_FILENAME> <STREAM-PAYMENT_FILENAME> <OUTPUT_FOLDER [default:./]>\n\n\
        The code imports batch-payment file (BATCH-PAYMENT_FILENAME) and constructs a graph using the provided transaction amongst users.\n\
        It then proceed to verify whether or not the incoming transactions (imported from STREAM-PAYMENT_FILENAME)\n\
        can be flagged as 'trusted' or 'unverified'.\n\
        Verification is evaluated given three features: \n\
            - feature 1: Users had a first-degree transaction(s): \"friends\". \n\
            - feature 2: Users have a common friend with whom they had a previous transaction(s): \"friends of a friend\"  \n\
            - feature 3: Users are included in a 5th-degree \"friendship\" network: \n\n \
        Input files: <BATCH-PAYMENT_FILENAME> and <STREAM-PAYMENT_FILENAME>\n\
            They should be provided in the *.csv (comma-separate-values) format.\n\
            Files should include a header defining the the following columns: \n\n\
               time, id1, id2, amount, message \n\
               + time: time and date of transaction \n\
               + id1: sender id [integer value] \n\
               + id2: receiver id [integer value] \n\
               + amount: amount of money sent by sender (id1) to the receiver (id2) ($US)\n\
               + message: message provided by sender (id1) at submission time \n\n\
            An example <BATCH-PAYMENT_FILENAME> or <STREAM-PAYMENT_FILENAME> file:\n\n\
                time, id1, id2, amount, message \n\
                2016-11-02 09:38:53, 52349, 8552, 37.10, Pitcher  \n\
                2016-11-02 09:38:54, 52345, 8550, 57.10, For stuff \n\n\
        Output files: \n\
            Three output files: output1.txt, output2.txt, and output3.txt will be created in <OUTPUT_FOLDER>.\n\
            If no <OUTPUT_FOLDER> is provided, current directory will be used.\n\
            Each line of output1.txt (using feature1) includes: \n\
                `trusted` or `unverified` corresponding to transactions provided in <STREAM-PAYMENT_FILENAME>\n";

#include <iostream>
#include <vector>
#include <deque>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <list>
#include "digitalwallet.h"





/* Let's begin */
int main(int argc, char *argv[])
{
    /* Check the number of parameters passed to main() via command line */
    if ( (argc != 3) && (argc != 4) ) {
        // Display the instructions & information 
        std::cerr << HELP << std::endl;
        return 1;
    } /* if argc */
    
    /* Start time profiler. Get current time stamp */ 
    TimeProfiler profiler("Time profiler: Start");

    /**********************************************************************/
    /* Import transaction files */
    std::string filename_batch(argv[1]);
    std::string filename_stream(argv[2]);
    Transactions *batch_payments = importCSV(filename_batch, ',');
    Transactions *stream_payments = importCSV(filename_stream, ',');
    profiler.addTimeStamp("Import transactions"); 

    /**********************************************************************/
    /* Setup graph using batch-payment transactions */
    int n_initial_graph = 1;  
    Graph g(n_initial_graph);
    g.addEdges(batch_payments->id_send, batch_payments->id_recv); 
    g.printSize();
    //g.printGraph();
    std::cout << "Graph setup successfully." << std::endl;
    profiler.addTimeStamp("Graph setup (batch payments)"); 

    /**********************************************************************/
    /* Setup fraud recommender using graph and number of features */
    FraudRecommender fraud_analyzer(NUMBER_OF_ALERT_FEATURES, FEATURE_3_NTH_DEGREE_FRIEND); 
    fraud_analyzer.setCustomersTransactionGraph(&g);
    std::cout << "Fraud analyzer setup successfully." << std::endl;
    profiler.addTimeStamp("Fraud recommender setup"); 
    
    /* Go through streamed transactions, analyze graph for each incoming transaction, */
    /* set three flags identifying transaction being trusted or unverified considering */
    /* feature1, feature2, and feature3.  */
    std::vector<std::vector< bool > > flags;
    Node *trans = new Node; 
    std::vector<bool> current_trans_flags(NUMBER_OF_ALERT_FEATURES,false);
    std::cout << "Start analyzing streamed-transactions..." << std::endl;
    for (size_t transaction_counter=0; transaction_counter < stream_payments->N; ++transaction_counter) 
    {
        if (transaction_counter % 10000 == 0) std::cout << "Processing transaction: "<< transaction_counter << std::endl;
        
        /* Get current transaction send/recv id's */
        trans->id_send = stream_payments->id_send[transaction_counter]; 
        trans->id_recv = stream_payments->id_recv[transaction_counter];  

        /* flags: results on fraud alert features (see problem description) */
        /* Special case: user sending money to himself/herself. Set all flags to true (trusted) */
        
        if ( isSendToSelf(trans) )
        {
            /* Set all the flags to true for current transaction */
            std::fill(current_trans_flags.begin(), current_trans_flags.end(), true);
            flags.push_back(current_trans_flags);
            continue; /* go to next transaction */
        } /* if id_send == id_recv */
        /* New user(s). Set all flags to false (unverified) */
        
        if (g.isNewNode(trans->id_send) || g.isNewNode(trans->id_recv) ) {
        
            std::fill(current_trans_flags.begin(), current_trans_flags.end(), false);
            flags.push_back(current_trans_flags);
            g.addEdge(trans->id_send, trans->id_recv);
            continue; /* go to next transaction */
        } 
        bool flag1 = false; 
        bool flag2 = false; 
        bool flag3 = false; 
        flag1 = fraud_analyzer.isVerifiedFeature_1(trans);
        if (flag1) { 
            flag2 = true; flag3 = true; 
        } else {
            flag2 = fraud_analyzer.isVerifiedFeature_2(trans);
            if (flag2) { 
                flag3 = true; 
            } else {
                flag3 = fraud_analyzer.isVerifiedFeature_3(trans);
            }
        }
        current_trans_flags[0] = flag1; 
        current_trans_flags[1] = flag2; 
        current_trans_flags[2] = flag3; 
        flags.push_back(current_trans_flags);
        g.addEdge(trans->id_send, trans->id_recv);
        
        //printTransactionFraudFlag(trans, flag1, flag2, flag3) ;
    } /* for loop */ 
    profiler.addTimeStamp("Fraud alert streamed-payment analysis"); 
    std::cout << "Finished analyzing streamed-transactions.\n";

/* Output results */
    std::string output_base_directory("./"); 
    if (argc == 4) {
        output_base_directory = argv[3];
    }

    std::cout << "Writing results for each feature...\n";
    writeFlags(output_base_directory, flags); 
    profiler.addTimeStamp("Output flags to file"); 

    delete batch_payments;
    delete stream_payments;
    delete trans;

/* Report time profiling */
    profiler.addTimeStamp("Final time"); 
    profiler.reportTimes(); 
    return 0;
}

