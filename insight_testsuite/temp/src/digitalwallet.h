#ifndef H_DIGITALWALLET
#define H_MYMATH 

const size_t NUMBER_OF_ALERT_FEATURES = 3;
const size_t FEATURE_3_NTH_DEGREE_FRIEND = 5;


/****************************************************************************************/
/****************************************************************************************/
/*                                        Structures                                    */
/****************************************************************************************/
/****************************************************************************************/

/* Single transaction between two users */
struct Node
{
    size_t id_send; 
    size_t id_recv; 
}; 

/* Transactions structure */
struct Transactions {

    std::vector<size_t> transaction_id; 
    std::vector<size_t> id_send; /* sender user id (id1) */
    std::vector<size_t> id_recv; /* receiver user id (id2) */
    std::vector<float> amount;    /* amount of money sent */
    std::vector<std::string> message; /* message given by sender */
    size_t N; /* Total number of transactions */
};

/****************************************************************************************/
/****************************************************************************************/
/*                                        Classes                                       */
/****************************************************************************************/
/****************************************************************************************/
/* Time profiler used in my code. */
class TimeProfiler
{
    std::vector<clock_t> time_stamps;
    std::vector<std::string> name_stamps; 
public: 

    TimeProfiler(std::string stamp_name);
    void addTimeStamp(std::string stamp_name);
    void reportTimes(void);
};

TimeProfiler::TimeProfiler(std::string stamp_name)
{

    addTimeStamp(stamp_name);
}

void TimeProfiler::addTimeStamp(std::string stamp_name)
{
    clock_t current_time = clock(); 
    time_stamps.push_back(current_time);
    name_stamps.push_back(stamp_name);
}

void TimeProfiler::reportTimes(void)
{
    std::cout << "\n\nSimulation Complete. Time analysis:\n" << std::endl;
    for (size_t i=0; i < time_stamps.size()-1; ++i) 
    {
        double dt = double(time_stamps[i+1] - time_stamps[i]) / CLOCKS_PER_SEC;
        std::cout << "Time spent in: " << name_stamps[i+1] <<": " << dt << " (sec)" << std::endl;
    } /* for */
}    
/*****************************************************************************************/

// This class represents an undirected graph using adjacency list representation
class Graph
{
    size_t V;    // No. of nodes (vertices)
    std::vector<std::vector< size_t> > adj; // list of nodes connected to each node. 
    // These vectors will be used with Breadth-First-Search Algorithm to find the 5-degree
    // friend in a given node's network.
    std::vector<bool> visited; /* to be used with BFS */
    std::vector<size_t> path_length; /* length of traveled path. To be used with BFS */
    /* Level depth used in BFS search algorithm. 5 in this problem */
    size_t feature_3_nth_level;
    
public:
    Graph(size_t V);  // Constructor
    void addEdge(size_t v, size_t w); // function to a single add edge (undirected)
    void addEdges(std::vector <size_t> v, std::vector <size_t> w);
    bool isNewNode(size_t v);
    void resizeVectors(void);
    void setConnected_NthLevel(size_t level);
    bool areNodesConnected_1stLevel(size_t v, size_t w);
    bool areNodesConnected_2ndLevel(size_t v, size_t w);
    bool areNodesConnected_NthLevel(size_t v, size_t w);
    void printSize(void);
    void printNode(size_t v);
    void printGraph(void);
};
 
Graph::Graph(size_t V)
{
    this->V = V;
    adj.resize(V);
    visited.resize(V, false); 
    path_length.resize(V, 0); 
    setConnected_NthLevel(FEATURE_3_NTH_DEGREE_FRIEND);
}

void Graph::printSize(void)
{
    std::cout<<"Graph has: "<<this->V<< " nodes." << std::endl;
}

/* Check if the node is a new node */
bool Graph::isNewNode(size_t v) {
    
    if (v > (adj.size()-1)) return true; 
    if (!adj[v].size()) return true; 
    return false; 
}

/* Add a vector of nodes edges to the graph */
void Graph::addEdges(std::vector <size_t> v, std::vector <size_t> w) 
{
    if ( v.size() != w.size() ) 
    {
        std::cout << "Error inserting nodes/edges to the graph. (id_send,id_recv) vectors are not the same size." << std::endl << std::endl;
        return;
    }/* if */
    for (size_t i=0; i != v.size(); ++i) 
    {
        addEdge(v[i], w[i]);         
    }
    std::cout << v.size() << " transactions added to the graph successfully." << std::endl << std::endl;
}

/* Add a single edge between nodes v and w to the graph */
void Graph::addEdge(size_t v, size_t w)
{
    size_t max_idx = std::max(v, w);
    bool insert = true; 
    bool resize = false;
    /* resize the adjacency std::vector first */
    if (max_idx > this->V-1) {
        this->V = max_idx+1;
        adj.resize(max_idx+1);
        resize = true;
        this->V = adj.size(); 

    } else {

        size_t idx, insert_val; 
        if ( adj[v].size() < adj[w].size() ) {
            idx = v; 
            insert_val = w; 
        } else {
            idx = w; 
            insert_val = v; 
        }
        
        /* Add only if the value is not already added */
        if ( std::find(adj[idx].begin(), adj[idx].end(), insert_val) != adj[idx].end() ) {
            insert = false; 
        }
    } /* else */
    if ( (insert) && (v != w) ){
        adj[v].push_back(w); // Add w to v’s list.
        adj[w].push_back(v); // Add v to w’s list.
    }
    if (resize) 
    {
        resizeVectors();
    }
}

void Graph::resizeVectors(void) {

    visited.resize(this->V, false); 
    path_length.resize(this->V, 0); 
}

void Graph::printNode(size_t v)
{
    std::cout<<"Node "<<v<<": [ ";
    for (size_t i=0; i<adj[v].size(); ++i) 
    {
        std::cout<<adj[v][i]<<" ";
    }
    std::cout<<"]"<<std::endl;
}

void Graph::printGraph(void) 
{
    std::cout<<"Printing graph: " << std::endl;
    for (size_t i=0; i<adj.size(); ++i) 
    {
        printNode(i);
    }
}

/* check if node v is directly connected to w */
bool Graph::areNodesConnected_1stLevel(size_t v, size_t w)
{
    return (std::find(adj[v].begin(), adj[v].end(), w) != adj[v].end());
}

/* check if nodes v and w are connected via a common node */
bool Graph::areNodesConnected_2ndLevel(size_t v, size_t w)
{
    return (std::find_first_of(adj[v].begin(), adj[v].end(), adj[w].begin(), adj[w].end()) != adj[v].end());
}

/* check if nodes v and w are connected via an N-th level network. Use Breadth-First-Search */
bool Graph::areNodesConnected_NthLevel(size_t v, size_t w)
{
    /* Breadth First Search Algorithm */
    size_t start = v; 
    size_t end = w;
    
    /* Mark all the vertices as not visited. */ 
    std::fill(visited.begin(), visited.end(), false);
    /* Set the traveled distance (from start point) to zero */
    std::fill(path_length.begin(), path_length.end(), 0);
    std::fill(path_length.begin(), path_length.end(), 0);
 
    // Create a queue for BFS
    std::deque<size_t> queue;
 
    // Mark the current node as visited and enqueue it
    visited[start] = true;
    queue.push_back(start);
     
    while(!queue.empty()) 
    {
        // Dequeue a vertex from queue and print it
        start = queue.front();
        queue.pop_front();
        
        // Get all adjacent vertices of the dequeued vertex
        // If a adjacent has not been visited, then mark it as visited
        // and enqueue it.
        // 'j': iterator to get all adjacent vertices of a vertex (node)
        std::vector<size_t>::iterator j;
        for(j = adj[start].begin(); j != adj[start].end(); ++j)
        {
            size_t node = *j; 
            if(!visited[node])
            {
                visited[node] = true;
                path_length[node] = path_length[start] + 1;
                if (end == node) {
                    return true;
                } else {
                    if (path_length[node] > this->feature_3_nth_level-1) {
                        return false;
                    }
                    queue.push_back(node);
                }
            }
        } /* for */
    }
    //std::cout<<"Could not find a path between ("<<v<<","<<w<<")"<<std::endl;
    return false; 
}

void Graph::setConnected_NthLevel(size_t level) 
{
    feature_3_nth_level = level; 
}
/*****************************************************************************************/

/* Class setup to analyze the batch-payment graph to suggest "trusted" or "unverified" 
for transactions. Currently, it uses three features discussed in the problem set */
class FraudRecommender
{
    int max_alerts; 
    int which_alert; 
    int feature3_connectivity_path_length; 
    Graph *graph;
    
public:
    FraudRecommender(int m, int feature3_degree);  // Constructor
    void setSystemGraph();
    bool isVerifiedFeature_1(Node *tr);
    bool isVerifiedFeature_2(Node *tr);
    bool isVerifiedFeature_3(Node *tr);
    void setCustomersTransactionGraph(Graph *gr);
};

FraudRecommender::FraudRecommender(int m, int feature3_degree) 
{
    /* number of alert analysis: set to three for this problem */
    this->max_alerts = m;
    /* feature 3: depth length (5: for the Data Challenge Problem). */
    this->feature3_connectivity_path_length = feature3_degree;
}

void FraudRecommender::setCustomersTransactionGraph(Graph *gr)
{
    graph = gr; 
}

bool FraudRecommender::isVerifiedFeature_1(Node *tr) {
    
    return ( graph->areNodesConnected_1stLevel(tr->id_send, tr->id_recv) );
}

bool FraudRecommender::isVerifiedFeature_2(Node *tr) {
    
    return ( graph->areNodesConnected_2ndLevel(tr->id_send, tr->id_recv) );
}

bool FraudRecommender::isVerifiedFeature_3(Node *tr) {
    
    return (graph->areNodesConnected_NthLevel(tr->id_send, tr->id_recv) );
}
/****************************************************************************************/
/****************************************************************************************/
/*                                        Functions                                     */
/****************************************************************************************/
/****************************************************************************************/

/* Import CSV file. Results are stored in transaction data structure */
Transactions *importCSV(std::string filename, const char delimiter) 
{

    Transactions *new_tr = new Transactions; 

    std::cout << "Importing file: '" << filename << "'" << std::endl;
    
    std::ifstream input_file(filename); 
    if (!input_file.is_open()) {
        std::cout << "Error opening file: '" << filename << "'" << std::endl;
        delete new_tr; 
        return NULL;
    } /* if file is opened correctly */

    std::string line;
    std::getline(input_file,line); /* skip the header line */
    int counter = 0; 
    int linenumber = 0;
    while(std::getline(input_file,line))
    {
        std::stringstream  singleLine(line);
        std::string        cell;

        int id1=-1;
        int id2=-1; 
        int item = 0;
        float amount=0.0;
        while(std::getline(singleLine, cell, delimiter))
        {
            if (item == 1) { /* id 1 */
                id1 = std::stoi(cell); 
            } else if (item == 2) { /* id 2 */
                id2 = std::stoi(cell);
            } else if (item == 3) {
                //amount = std::stof(cell); 
                break;
            }
            item++; 
        }
        new_tr->id_send.push_back((size_t)id1); 
        new_tr->id_recv.push_back((size_t)id2); 
        //new_tr->amount.push_back(amount); 
        linenumber++; 
    } 
    std::cout << "A total of " << linenumber << " was imported successfully" << std::endl; 
    new_tr->N = linenumber;
    
    return new_tr;
}
/*****************************************************************************************/

/* This function writes flags for each feature to output1.txt, output2.txt and output3.txt*/
void writeFlags(std::string output_write_directory, std::vector<std::vector<bool> > flags) 
{
    for (size_t feature=1; feature <= NUMBER_OF_ALERT_FEATURES; ++feature) {
    
        std::string out_filename = output_write_directory + "/output" + std::to_string(feature) + ".txt";
        std::ofstream file(out_filename);
        if (!file.is_open()) {
            std::cerr << "Error opening " << out_filename << " " << std::strerror(errno) << std::endl;
            return;
        } /* if file is opened correctly */
        for (size_t i=0; i != flags.size(); ++i) 
        {
            if (flags[i][feature-1])
            {
                file << "trusted\n";
            
            } else
            {
                file << "unverified\n";
            }
        } /* for */
        file.close();
        file.clear(); 
        std::cout << "Flags feature-"<< feature << " have been written to " << out_filename << " successfully." << std::endl; 
    } /* for feature */
}
/*****************************************************************************************/

/* Check if user is sending money to himself/herself */
inline bool isSendToSelf(Node *send_recv) 
{
    if (send_recv->id_send == send_recv->id_recv) {
        return true; 
    } else {
        return false; 
    }
}
/*****************************************************************************************/


#endif
















