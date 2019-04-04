#include <queue>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include "binstr.h"
using namespace std;


//node of the Huffman tree
struct TreeNode
{
    unsigned short num, children[2];
    unsigned freq;
};


//compare the weight of two node
bool operator<(const TreeNode &lhs, const TreeNode &rhs)
{
    return lhs.freq > rhs.freq || (lhs.freq == rhs.freq && lhs.num < rhs.num);
}


//add a new tree node to the Huffman tree
TreeNode add_leaf_node(std::vector<TreeNode> &the_tree, unsigned freq)
{
    TreeNode ret;
    ret.freq = freq;
    ret.num = the_tree.size();
    the_tree.push_back(ret);
    return ret;
}


TreeNode add_merged_nodes(std::vector<TreeNode> &the_tree,
                          const TreeNode &lhs, const TreeNode &rhs)
{
    TreeNode ret;
    ret.freq = lhs.freq + rhs.freq;
    ret.children[0] = lhs.num;
    ret.children[1] = rhs.num;
    ret.num = the_tree.size();
    the_tree.push_back(ret);
    return ret;
}


void recur(size_t cur, const std::vector<TreeNode> &the_tree, BinStr *convert_table[], BinStr &tmp)
{
    if (cur < 256)
    {
        convert_table[cur] = new BinStr(tmp);
    }
    else
    {
        tmp.push_back(0, 1);
        recur(the_tree[cur].children[0], the_tree, convert_table, tmp);
        tmp.pop_bit();
        tmp.push_back(1, 1);
        recur(the_tree[cur].children[1], the_tree, convert_table, tmp);
        tmp.pop_bit();
    }
}


char *compress(const char *input, size_t insize, size_t &outsize)
{
    // build tree
    unsigned freq[256];
    memset(freq, 0, sizeof(unsigned) * 256);

    for (size_t i = 0; i < insize; i++)
    {
        freq[(unsigned char)input[i]]++;
    }
    std::vector<TreeNode> the_tree;

    std::priority_queue<TreeNode> the_heap;
    for (size_t i = 0; i < 256; i++)
    {
        TreeNode tmp = add_leaf_node(the_tree, freq[i]);
        if (freq[i])
        {
            the_heap.push(tmp);
        }
    }
    while (the_heap.size() > 1)
    {
        TreeNode a = the_heap.top();
        the_heap.pop();
        TreeNode b = the_heap.top();
        the_heap.pop();
        the_heap.push(add_merged_nodes(the_tree, a, b));
    }
    // build convert table
    BinStr *convert_table[256];
    memset(convert_table, 0, sizeof(BinStr *) * 256);
    BinStr tmp;
    recur(the_tree.size() - 1, the_tree, convert_table, tmp);
    // start dumping
    BinStr output;
    output.push_back(the_tree.size() - 256, 8);
    for (size_t i = 256; i < the_tree.size(); i++)
    {
        output.push_back(the_tree[i].children[0], 9);
        output.push_back(the_tree[i].children[1], 9);
    }
    for (size_t i = 0; i < insize; i++)
    {
        output.push_back(*convert_table[(unsigned char)input[i]]);
    }
    for (size_t i = 0; i < 256; i++)
    {
        delete convert_table[i];
    }
    output.fill();
    return output.copy_to_buffer(outsize);
}


char *decompress(const char *input, size_t inlen, size_t &outlen)
{

    //tmp for real content
    BinStr compressed((uint8_t *)input, inlen);
    //delete the padding
    compressed.unfill();
    //rebuild the tree
    size_t cnt_nonleaf = compressed.substr(0, 8);
    std::vector<TreeNode> the_tree;
    size_t index = 8;
    for (size_t i = 0; i < cnt_nonleaf; i++)
    {
        TreeNode tmp;
        tmp.children[0] = compressed.substr(index, 9);
        index += 9;
        tmp.children[1] = compressed.substr(index, 9);
        index += 9;
        the_tree.push_back(tmp);
    }
    //start decoding
    BinStr decompressed;
    size_t cur, root_node;
    cur = root_node = cnt_nonleaf + 255;
    while (index < compressed.get_bit_size())
    {
        int bit = compressed.substr(index, 1);
        cur = the_tree[cur - 256].children[bit];
        if (cur < 256)
        {
            decompressed.push_back(cur, 8);
            cur = root_node;
        }
        index++;
    }
    //turn the buffer into char
    return decompressed.copy_to_buffer(outlen);
}

int main()
{
    std::ifstream is("samples/news.docx", std::ios::binary);
    char *initial = new char[600000];
    is.read(initial, 600000);

    size_t inlen = is.gcount(), outlen;
    char *buf = compress(initial, inlen, outlen);
    std::cout << inlen << std::endl;
    std::cout << outlen << std::endl;
    char *debuf = decompress(buf, outlen, inlen);
    std::ofstream decode("samples/decode.docx", std::ios::binary);
    decode.write(debuf, inlen);
    delete[] debuf;
    delete[] buf;
    delete[] initial;

    return 0;
}
