//
// Created by GuoZL on 2023-02-22.
//

#include "utils.h"

#include <bits/stdc++.h>

using namespace std;


#define DEBUG
#define DEFAULT_IMG_FN "../test.jpg"

// alert: assuming that the image file is no larger than 2147483647 Bytes (~2GB)
tuple<char *, int> ApproachOne(const string &img_path);
tuple<char *, int> ApproachTwo(const string &img_path);

tuple<char *, int> MakeImageStreamBufferFromFile() {
    // compatability alert: C++ 11 or up
    return ApproachOne(DEFAULT_IMG_FN);
    // return ApproachTwo(DEFAULT_IMG_FN);
}

tuple<char *, int> MakeImageStreamBufferFromFile(const string &img_path) {
    // compatability alert: C++ 11 or up
    return ApproachOne(img_path);
    // return ApproachTwo(img_path);
}


tuple<char *, int> ApproachOne(const string &img_path) {
    // compatability alert: C++ 11 or up
    // reference: https://blog.csdn.net/qq_29695701/article/details/84262492
    // [alert] Stack Overflow discourages the use of `tellg()`. per: https://stackoverflow.com/a/13394183

    // read-binary
    ifstream img_stream(img_path, ifstream::in | ios::binary);

    // calculate the length of the image
    img_stream.seekg(0, std::ifstream::end);
    int length = (int) img_stream.tellg();
    img_stream.seekg(0, std::ifstream::beg);

    if (length <= 0) {  // error handling
        fprintf(stderr, "[ERROR] Failed to Read Image \"%s\". Length=%d<=0 \n", DEFAULT_IMG_FN, length);
        exit(1);
    }
    // declare & allocate memory buffer
    char *buffer = new char[length];
    // read image & store to buffer
    img_stream.read(buffer, length);

    // release file descriptor
    img_stream.close();

    cout << "Read & Store Image Sized " << length << " Bytes to Memory Buffer. (Approach #1)" << endl;
    return make_tuple(buffer, length);
}

tuple<char *, int> ApproachTwo(const string &img_path) {
    // compatability alert: C++ 11 or up
    // reference: https://blog.csdn.net/weixin_34108829/article/details/117040951

    int length;
    FILE *fp;

    // read-binary
    if (nullptr == (fp = fopen(img_path.c_str(), "rb"))) {
        fprintf(stderr, "[ERROR] Failed to Read Image \"%s\".\n", DEFAULT_IMG_FN);
        exit(1);
    }

    // calculate the length of the image
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);

    // declare & allocate memory buffer
    char *buffer = (char *) malloc(length * sizeof(char));
    // read image & store to buffer
    fread(buffer, length, 1, fp);

    // release file descriptor
    fclose(fp);

    cout << "Read & Store Image Sized " << length << " Bytes to Memory Buffer. (Approach #2)" << endl;
    return make_tuple(buffer, length);
}