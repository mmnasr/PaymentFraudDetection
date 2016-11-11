# Payment Fraud Detection
### Insight Data Program: Data Engineering Challenge
### By: Mohamad Nasr-Azadani, Email: mmnasr@gmail.com

## Requirements: 
### Code is prepared in C++ with OOP.
### Compiler version: C++11 is required

## General description and usage:

<./EXEC_FILE> \<BATCH-PAYMENT_FILENAME\> \<STREAM-PAYMENT_FILENAME\> \<OUTPUT_FOLDER [default:./]\>
The code imports batch-payment file (BATCH-PAYMENT_FILENAME) and constructs a graph using the provided transaction amongst users.
It then proceed to verify whether or not the incoming transactions (imported from STREAM-PAYMENT_FILENAME) can be flagged as 'untrusted' or 'verified'.

Verification is evaluated given three features: 
    - feature 1: Users had a first-degree transaction(s): \"friends\". 
    - feature 2: Users have a common friend with whom they had a previous transaction(s): \"friends of a friend\"  
    - feature 3: Users are included in a 5th-degree \"friendship\" network: 
            
Input files: <BATCH-PAYMENT_FILENAME> and <STREAM-PAYMENT_FILENAME>
      They should be provided in the *.csv (comma-separate-values) format.
      Files should include a header defining the the following columns: 
               time, id1, id2, amount, message 
               + time: time and date of transaction 
               + id1: sender id [integer value] 
               + id2: receiver id [integer value] 
               + amount: amount of money sent by sender (id1) to the receiver (id2) ($US)
               + message: message provided by sender (id1) at submission time 

An example <BATCH-PAYMENT_FILENAME> or <STREAM-PAYMENT_FILENAME> file:
        time, id1, id2, amount, message 
        2016-11-02 09:38:53, 52349, 8552, 37.10, Pitcher  
        2016-11-02 09:38:54, 52345, 8550, 57.10, For stuff 
Output files: 
        Three output files: output1.txt, output2.txt, and output3.txt will be created in <OUTPUT_FOLDER>.
        If no <OUTPUT_FOLDER> is provided, current directory will be used.
        Each line of output1.txt (using feature1) includes: 
            `untrusted` or `verified` corresponding to transactions provided in <STREAM-PAYMENT_FILENAME>

### For more information, check problem description [link](https://github.com/InsightDataScience/digital-wallet/blob/master/README.md).

