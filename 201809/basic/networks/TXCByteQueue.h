//
// Created by alderzhang on 2017/6/30.
//

#ifndef LITEAV_TXCBYTEQUEUE_H
#define LITEAV_TXCBYTEQUEUE_H


class TXCByteQueue {
public:
	TXCByteQueue(long capacity);

	virtual ~TXCByteQueue();

public:
	static TXCByteQueue *clone(const void *bytes, long length);

	long available();

	long length();

	void clear();

	bool putByte(unsigned char aByte);

	bool putBytes(const void *bytes, long length);

	int getByte();

	bool getBytes(void *dst, long length);

	void getAll(void *dst);

	int peekAt(long offset);

	int peekByte();

	bool peekBytes(void *dst, long length);

	void peekAll(void *dst);

	void skip(long length);

	const unsigned char *dataBuffer();

	long dataBufferLength();

	char readChar();

	unsigned char readUChar();

	short readShort();

	unsigned short readUShort();

	int readInt();

	unsigned int readUInt();

	long long readLong();

	unsigned long long readULong();

	bool writeChar(char aChar);

	bool writeUChar(unsigned char aUChar);

	bool writeShort(short aShort);

	bool writeUShort(unsigned short aUShort);

	bool writeInt(int aInt);

	bool writeUInt(unsigned int aUInt);

	bool writeLong(long long aLong);

	bool writeULong(unsigned long long aULong);

	bool writeString(const char *str);

private:
	long getHead(long index);

	long increaseHead(long length);

	long increaseTail(long length);

private:
	unsigned char *_bytes;
	long _head;
	long _tail;
	long _capacity;
};


#endif //LITEAV_TXCBYTEQUEUE_H
