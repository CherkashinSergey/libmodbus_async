#ifndef TEST_H
#define TEST_H

#define SERVER_ID         17
#define TEST_RET_VALUE    37
#define AMOUNT_OF_DEVICES 5
#define PORT_RANGE_BEGIN  1503


#define ASSERT_TRUE(_cond, _format, __args...) {  \
    if (_cond) {                                  \
        printf("OK\n");                           \
    } else {                                      \
        printf("NOT OK\n");                       \
    }                                             \
};

#endif /*TEST_H*/