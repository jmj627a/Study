#include "RingBuf.h"
#include <string>
#include <Windows.h>

void RingBuf::Initialize(int iBufferSize)
{
	Finalize();

	if (iBufferSize > 0)
	{
		dataBuf = new char[iBufferSize];
		memset(dataBuf, 0, iBufferSize);
		readPos = 0;
		writePos = 0;
		totalSize = iBufferSize;
		usedSize = 0;
	}
}

void RingBuf::Finalize(void)
{
	if (dataBuf != NULL)
	{
		delete[] dataBuf;
	}

	dataBuf = NULL;
	readPos = 0;
	writePos = 0;
	totalSize = 0;
	usedSize = 0;
}

RingBuf::RingBuf(void)
{
	dataBuf = NULL;
	readPos = 0;
	writePos = 0;
	totalSize = 0;
	usedSize = 0;

	Initialize(200);
}

RingBuf::RingBuf(int iBufferSize)
{
	dataBuf = NULL;
	readPos = 0;
	writePos = 0;
	totalSize = 0;
	usedSize = 0;

	Initialize(iBufferSize);
}

void RingBuf::Resize(int iBufferSize)
{
	Initialize(iBufferSize);
}

int RingBuf::GetBufferSize(void)
{
	return totalSize;
}

int RingBuf::GetUseSize(void)
{
	return usedSize;
}

int RingBuf::GetFreeSize(void)
{
	return (totalSize - usedSize);
}

int RingBuf::Enqueue(char* chpData, int iSize)
{
	//더 넣을 용량이 없으면 나가기
	if (GetFreeSize() == 0)
		return 0;

	if (readPos <= writePos)
	{

		int rearSize = totalSize - writePos;

		if (rearSize < iSize)
		{
			memcpy((dataBuf + writePos), chpData, rearSize);

			int frontSize = readPos;
			int remainSize = iSize - rearSize;

			if (remainSize <= frontSize)
			{
				memcpy(dataBuf, (chpData + rearSize), remainSize);

				writePos = remainSize;
				usedSize += iSize;

				return iSize;
			}
			else
			{
				memcpy(dataBuf, (chpData + rearSize), frontSize);

				writePos = frontSize;
				usedSize += (frontSize + rearSize);

				return (frontSize + rearSize);
			}
		}
		else
		{
			memcpy((dataBuf + writePos), chpData, iSize);

			writePos = (writePos + iSize) % totalSize;
			usedSize += iSize;

			return iSize;
		}
	}
	else
	{
		int remainSize = readPos - writePos;

		if (remainSize < iSize)
		{
			memcpy((dataBuf + writePos), chpData, remainSize);

			writePos = (writePos + remainSize) % totalSize;
			usedSize += remainSize;

			return remainSize;
		}
		else
		{
			memcpy((dataBuf + writePos), chpData, iSize);

			writePos = (writePos + iSize) % totalSize;
			usedSize += iSize;

			return iSize;
		}
	}

	return 0;
}

int RingBuf::Dequeue(char* chpData, int iSize)
{
	//더 뺄 게 없으면 나가기
	if (usedSize == 0)
		return 0;

	if (readPos < writePos)
	{
		int remainSize = writePos - readPos;

		if (remainSize < iSize)
		{
			memcpy(chpData, (dataBuf + readPos), remainSize);

			readPos = (readPos + remainSize) % totalSize;
			usedSize -= remainSize;

			return remainSize;
		}
		else
		{
			memcpy(chpData, (dataBuf + readPos), iSize);

			readPos = (readPos + iSize) % totalSize;
			usedSize -= iSize;

			return iSize;
		}
	}
	//이제 여기서 같다고 나오는 경우는 버퍼가 꽉 찬 경우 뿐
	else if(readPos >= writePos)
	{
		int rearSize = totalSize - readPos;

		if (rearSize < iSize)
		{
			memcpy(chpData, (dataBuf + readPos), rearSize);

			int frontSize = writePos;
			int remainSize = iSize - rearSize;

			if (remainSize <= frontSize)
			{
				memcpy((chpData + rearSize), dataBuf, remainSize);

				readPos = remainSize;
				usedSize -= iSize;

				return iSize;
			}
			else
			{
				memcpy((chpData + rearSize), dataBuf, frontSize);

				readPos = frontSize;
				usedSize -= (frontSize + rearSize);

				return (frontSize + rearSize);
			}
		}
		else
		{
			memcpy(chpData, (dataBuf + readPos), iSize);

			readPos = (readPos + iSize) % totalSize;
			usedSize -= iSize;

			return iSize;
		}
	}

	return 0;
}

int RingBuf::Peek(char* chpData, int iSize)
{
	if (usedSize == 0)
		return 0;

	if (readPos < writePos)
	{
		int remainSize = writePos - readPos;

		if (remainSize < iSize)
		{
			memcpy(chpData, (dataBuf + readPos), remainSize);

			return remainSize;
		}
		else
		{
			memcpy(chpData, (dataBuf + readPos), iSize);

			return iSize;
		}
	}
	//이제 여기서 같다고 나오는 경우는 버퍼가 꽉 찬 경우 뿐
	else if (readPos >= writePos)
	{
		int rearSize = totalSize - readPos;

		if (rearSize < iSize)
		{
			memcpy(chpData, (dataBuf + readPos), rearSize);

			int frontSize = writePos;
			int remainSize = iSize - rearSize;

			if (remainSize <= frontSize)
			{
				memcpy((chpData + rearSize), dataBuf, remainSize);

				return iSize;
			}
			else
			{
				memcpy((chpData + rearSize), dataBuf, frontSize);

				return (frontSize + rearSize);
			}
		}
		else
		{
			memcpy(chpData, (dataBuf + readPos), iSize);

			return iSize;
		}
	}

	return 0;
}

int RingBuf::MoveRear(int iSize)
{
	int freeSize = GetFreeSize();

	if (iSize < freeSize)
	{
		writePos = (writePos + iSize) % totalSize;
		usedSize += iSize;

		return iSize;
	}
	else
	{
		writePos = (writePos + freeSize) % totalSize;
		usedSize += freeSize;

		return freeSize;
	}

	return 0;
}

int RingBuf::MoveFront(int iSize)
{
	int useSize = GetUseSize();

	if (iSize < useSize)
	{
		readPos = (readPos + iSize) % totalSize;
		usedSize -= iSize;

		return iSize;
	}
	else
	{
		readPos = (readPos + useSize) % totalSize;
		usedSize -= useSize;

		return useSize;
	}

	return 0;
}

void RingBuf::ClearBuffer(void)
{
	readPos = 0;
	writePos = 0;
	usedSize = 0;
}

char* RingBuf::GetFrontBufferPtr(void)
{
	return (dataBuf + readPos);
}

char* RingBuf::GetRearBufferPtr(void)
{
	return (dataBuf + writePos);
}

char* RingBuf::GetBufferPtr(void)
{
	return dataBuf;
}

int RingBuf::DirectEnqueueSize(void)
{
	if (readPos <= writePos)
		return (totalSize - writePos);
	else
		return (readPos - writePos);
}

int RingBuf::DirectDequeueSize(void)
{
	if (readPos <= writePos)
		return (writePos - readPos);
	else
		return (totalSize - readPos);
}

void RingBuf::PrintBufState(void)
{
	printf("[Buffer State] R:%4d / W:%4d / T:%4d / U:%4d / F:%4d\n",
		readPos, writePos, totalSize, usedSize, GetFreeSize());
}
