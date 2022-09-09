#pragma once
#ifndef JEDECVENDOR_H
#define JEDECVENDOR_H

const char* jedecDecoderSequental(unsigned char* bytes);
const char* jedecDecoderIndexed(int group, int index);

#endif  // JEDECVENDOR_H