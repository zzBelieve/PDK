#include "PDKGameLogic.h"

using namespace utility;

FV_SINGLETON_STORAGE(PDKGameLogic);

bool SortCompare(const BYTE& cbCard1,const BYTE& cbCard2)
{
	BYTE nValue1 = PDKGameLogic::Instance().GetCardValue(cbCard1);
	BYTE nValue2 = PDKGameLogic::Instance().GetCardValue(cbCard2);

	return nValue1>nValue2;
}
//////////////////////////////////////////////////////////////////////////

//�˿�����
const BYTE PDKGameLogic::m_cbCardListData[48]=
{
	0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,// 0x0F,	//���� 3 -  A -2
	0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,// 0x1F,	//÷�� 3 -  A -2
	0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,// 0x2F,	//���� 3 -  A -2
	0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,/*0x3E,*/0x3F,	//���� 3 -  A -2
};

//////////////////////////////////////////////////////////////////////////

//���캯��
PDKGameLogic::PDKGameLogic()
{
}

//��������
PDKGameLogic::~PDKGameLogic()
{
}

//��ȡ����
BYTE PDKGameLogic::GetCardType(BYTE bCardData[], BYTE bCardCount)
{
	//������
	switch (bCardCount)
	{
	case 1: //����
		{
			return CT_SINGLE;
		}
	case 2:	//����
		{
			return (GetCardLogicValue(bCardData[0])==GetCardLogicValue(bCardData[1]))?CT_DOUBLE_LINE:CT_ERROR;
		}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(bCardData,bCardCount,AnalyseResult);

	//ը���ж�
	if ((AnalyseResult.bFourCount==1)&&(bCardCount==4)) return CT_BOMB;

    //�Ĵ����ж�
    if ((AnalyseResult.bFourCount==1)&&(bCardCount==7))
    {
        return CT_FOUR_LINE_TAKE_THREE;
    }
    
	//�����ж�
	if (AnalyseResult.bThreeCount>0)
	{
		//�����ж�
		bool bSeriesCard=false;
		if ((AnalyseResult.bThreeCount==1)||(AnalyseResult.bThreeLogicVolue[0]!=15))
		{
			int i = 0;
			for (i=1;i<AnalyseResult.bThreeCount;i++)
			{
				if (AnalyseResult.bThreeLogicVolue[i]!=(AnalyseResult.bThreeLogicVolue[0]-i)) break;
			}
			if (i==AnalyseResult.bThreeCount) bSeriesCard=true;
		}

		//�����ж�
		if (bSeriesCard==true)
		{
			//���ݶ���
			BYTE bSignedCount=bCardCount-AnalyseResult.bThreeCount*3;

			//���ͷ���
            if ((AnalyseResult.bThreeCount*3)==bCardCount) return CT_THREE_LINE;
            
            if((AnalyseResult.bThreeCount == 1) && (bSignedCount == 1) && (bCardCount == 4))return CT_THREE_LINE_TAKE_SINGLE;
            
            if(((bSignedCount+AnalyseResult.bThreeCount)==(AnalyseResult.bThreeCount*3)) && ((bSignedCount + (AnalyseResult.bThreeCount*3)) == bCardCount))
                return CT_THREE_LINE_TAKE_DOUBLE;
            if((AnalyseResult.bFourCount > 0) && (bSignedCount-(AnalyseResult.bFourCount*4) + AnalyseResult.bFourCount)==((AnalyseResult.bFourCount+AnalyseResult.bThreeCount) * 2))
                return CT_THREE_LINE_TAKE_DOUBLE;
            if(AnalyseResult.bThreeCount >= 2)
                return CT_THREE_LINE_TAKE_DOUBLE;
        }
	}

	//�����ж�
	if (AnalyseResult.bDoubleCount>0)
	{
		//�����ж�
		bool bSeriesCard=false;
		if ((AnalyseResult.bDoubleCount==1)||(AnalyseResult.bDoubleLogicVolue[0]!=15))
		{
			int i = 0;
			for (i=1;i<AnalyseResult.bDoubleCount;i++)
			{
				if (AnalyseResult.bDoubleLogicVolue[i]!=(AnalyseResult.bDoubleLogicVolue[0]-i)) break;
			}
			if (i==AnalyseResult.bDoubleCount) bSeriesCard=true;
		}

		//�����ж�
		if ((bSeriesCard==true)&&(AnalyseResult.bDoubleCount*2==bCardCount)) return CT_DOUBLE_LINE;
	}

	//�����ж�
	if ((AnalyseResult.bSignedCount>=5)&&(AnalyseResult.bSignedCount==bCardCount))
	{
		//��������
		bool bSeriesCard=false;
		BYTE bLogicValue=GetCardLogicValue(bCardData[0]);

		//�����ж�
		if (bLogicValue!=15)
		{
			int i = 0;
			for ( i=1;i<AnalyseResult.bSignedCount;i++)
			{
				if (GetCardLogicValue(bCardData[i])!=(bLogicValue-i)) break;
			}
			if (i==AnalyseResult.bSignedCount) bSeriesCard=true;
		}

		//�����ж�
		if (bSeriesCard==true) return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}


BYTE PDKGameLogic::GetCardType(const std::vector<BYTE>& cbCardList)
{
	int bCardCount = cbCardList.size();
	//������
	switch (bCardCount)
	{
	case 1: //����
		{
			return CT_SINGLE;
		}
	case 2:	//����
		{
			return (GetCardLogicValue(cbCardList[0])==GetCardLogicValue(cbCardList[1]))?CT_DOUBLE_LINE:CT_ERROR;
		}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardList,AnalyseResult);

	//ը���ж�
	if ((AnalyseResult.bFourCount==1)&&(bCardCount==4)) return CT_BOMB;
    
    //�Ĵ����ж�
    if ((AnalyseResult.bFourCount==1)&&((bCardCount==7)||(bCardCount==6)))
    {
        return CT_FOUR_LINE_TAKE_THREE;
    }
    
	//�����ж�
	if (AnalyseResult.bThreeCount>0)
	{
		//�����ж�
		bool bSeriesCard=false;
		if ((AnalyseResult.bThreeCount==1)||(AnalyseResult.bThreeLogicVolue[0]!=15))
		{
			int i = 0;
			for (i=1;i<AnalyseResult.bThreeCount;i++)
			{
				if (AnalyseResult.bThreeLogicVolue[i]!=(AnalyseResult.bThreeLogicVolue[0]-i)) break;
			}
			if (i==AnalyseResult.bThreeCount) bSeriesCard=true;
            //���ڷɻ�,������������
            if (i > 1)bSeriesCard=true;
		}

		//�����ж�
		if (bSeriesCard==true)
		{
			//���ݶ���
			BYTE bSignedCount=bCardCount-AnalyseResult.bThreeCount*3;
//			BYTE bDoubleCount=AnalyseResult.bDoubleCount+AnalyseResult.bFourCount*2;
            
			//���ͷ���
            if ((AnalyseResult.bThreeCount*3)==bCardCount) return CT_THREE_LINE;
            
            if((AnalyseResult.bThreeCount == 1) && (bSignedCount == 1) && (bCardCount == 4))return CT_THREE_LINE_TAKE_SINGLE;
            
            if(((bSignedCount+AnalyseResult.bThreeCount)==(AnalyseResult.bThreeCount*3)) && ((bSignedCount + (AnalyseResult.bThreeCount*3)) == bCardCount))
                return CT_THREE_LINE_TAKE_DOUBLE;
            if((AnalyseResult.bFourCount > 0) && (bSignedCount-(AnalyseResult.bFourCount*4) + AnalyseResult.bFourCount)==((AnalyseResult.bFourCount+AnalyseResult.bThreeCount) * 2))
                return CT_THREE_LINE_TAKE_DOUBLE;
            if(AnalyseResult.bThreeCount >= 2)
                return CT_THREE_LINE_TAKE_DOUBLE;
		}
	}

	//�����ж�
	if (AnalyseResult.bDoubleCount>0)
	{
		//�����ж�
		bool bSeriesCard=false;
		if ((AnalyseResult.bDoubleCount==1)||(AnalyseResult.bDoubleLogicVolue[0]!=15))
		{
			int i = 0;
			for (i=1;i<AnalyseResult.bDoubleCount;i++)
			{
				if (AnalyseResult.bDoubleLogicVolue[i]!=(AnalyseResult.bDoubleLogicVolue[0]-i)) break;
			}
			if (i==AnalyseResult.bDoubleCount) bSeriesCard=true;
		}

		//�����ж�
		if ((bSeriesCard==true)&&(AnalyseResult.bDoubleCount*2==bCardCount)) return CT_DOUBLE_LINE;
	}

	//�����ж�
	if ((AnalyseResult.bSignedCount>=5)&&(AnalyseResult.bSignedCount==bCardCount))
	{
		//��������
		bool bSeriesCard=false;
		BYTE bLogicValue=GetCardLogicValue(cbCardList[0]);

		//�����ж�
		if (bLogicValue!=15)
		{
			int i = 0;
			for ( i=1;i<AnalyseResult.bSignedCount;i++)
			{
				if (GetCardLogicValue(cbCardList[i])!=(bLogicValue-i)) break;
			}
			if (i==AnalyseResult.bSignedCount) bSeriesCard=true;
		}

		//�����ж�
		if (bSeriesCard==true) return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}


//�����˿�
void PDKGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount)
{
	//ת����ֵ
	BYTE bLogicVolue[MAX_COUNT];
	for (BYTE i=0;i<bCardCount;i++)	bLogicVolue[i]=GetCardLogicValue(bCardData[i]);	

	//�������
	bool bSorted=true;
	BYTE bTempData,bLast=bCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((bLogicVolue[i]<bLogicVolue[i+1])||
				((bLogicVolue[i]==bLogicVolue[i+1])&&(bCardData[i]<bCardData[i+1])))
			{
				//����λ��
				bTempData=bCardData[i];
				bCardData[i]=bCardData[i+1];
				bCardData[i+1]=bTempData;
				bTempData=bLogicVolue[i];
				bLogicVolue[i]=bLogicVolue[i+1];
				bLogicVolue[i+1]=bTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

void PDKGameLogic::SortCardList(std::vector<BYTE>& cbCardList)
{
	std::sort(cbCardList.begin(),cbCardList.end(),SortCompare);
}
//�����˿�
void PDKGameLogic::RandCardList(BYTE bCardBuffer[], BYTE bBufferCount)
{
	//����׼��
	BYTE bCardData[sizeof(m_cbCardListData)];
	memcpy(bCardData,m_cbCardListData,sizeof(m_cbCardListData));

	srand((DWORD)(time(NULL)));
	//�����˿�
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(bBufferCount-bRandCount);
		bCardBuffer[bRandCount++]=bCardData[bPosition];
		bCardData[bPosition]=bCardData[bBufferCount-bRandCount];
	} while (bRandCount<bBufferCount);

	return;
}

void PDKGameLogic::RandCardList(std::vector<BYTE>& cbCardList)
{
	//����׼��
	//memcpy(cbCardBuffer,m_cbCardListData,cbBufferCount);
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardListData)];
	memcpy(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	srand(DWORD(time(NULL)));
	WORD nCardCount = CountArray(m_cbCardListData);
	cbCardList.clear();
	//�����˿�
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(nCardCount-bRandCount);
		cbCardList.push_back(cbCardData[bPosition]);
		bRandCount++;
		cbCardData[bPosition]=cbCardData[nCardCount-bRandCount];
	} while (bRandCount<nCardCount);
}

//ɾ���˿�
bool PDKGameLogic::RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE bCardCount)
{
	//��������
	ASSERT(bRemoveCount<=bCardCount);

	//�������
	BYTE bDeleteCount=0,bTempCardData[20];
	if (bCardCount>CountArray(bTempCardData)) return false;
	memcpy(bTempCardData,bCardData,bCardCount*sizeof(bCardData[0]));

	//�����˿�
	for (BYTE i=0;i<bRemoveCount;i++)
	{
		for (BYTE j=0;j<bCardCount;j++)
		{
			if (bRemoveCard[i]==bTempCardData[j])
			{
				bDeleteCount++;
				bTempCardData[j]=0;
				break;
			}
		}
	}
	if (bDeleteCount!=bRemoveCount) return false;

	//�����˿�
	BYTE bCardPos=0;
	for (BYTE i=0;i<bCardCount;i++)
	{
		if (bTempCardData[i]!=0) bCardData[bCardPos++]=bTempCardData[i];
	}

	return true;
}


//ɾ���˿�
bool PDKGameLogic::RemoveCard(const std::vector<BYTE>& kRemoveCardList, std::vector<BYTE>& kCardList)
{
	int bRemoveCount = kRemoveCardList.size();
	int bCardCount = kCardList.size();
	//��������
	ASSERT(bRemoveCount<=bCardCount);

	int bDeleteCount = 0;
	//�����˿�
	for (BYTE i=0;i<bRemoveCount;i++)
	{
		for (BYTE j=0;j<bCardCount;j++)
		{
			if (kRemoveCardList[i]==kCardList[j])
			{
				bDeleteCount++;
				kCardList[j]=0;
				break;
			}
		}
	}

	if (bDeleteCount!=bRemoveCount) return false;
	std::vector<BYTE>::iterator iter = kCardList.begin();
	while(iter != kCardList.end())
	{
		BYTE cbCardData = *iter;
		if (cbCardData == 0)
		{
			iter = kCardList.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	return true;
}


//��Ч�ж�
bool PDKGameLogic::IsValidCard(BYTE cbCardData)
{
	//��������
	BYTE cbColor=GetCardColor(cbCardData);
	BYTE cbValue=GetCardValue(cbCardData);

	//��Ч�ж�
	switch (cbColor)
	{
	case 0x00:
		{
			return ((cbValue>=0x03)&&(cbValue<=0x0D));
		}
	case 0x10:
		{
			return ((cbValue==0x01)||((cbValue>=0x03)&&(cbValue<=0x0D)));
		}
	case 0x20:
		{
			return ((cbValue==0x01)||((cbValue>=0x03)&&(cbValue<=0x0D)));
		}
	case 0x30:
		{
			return ((cbValue>=0x01)&&(cbValue<=0x0D));
		}
	}

	return false;
}

//�߼���ֵ
BYTE PDKGameLogic::GetCardLogicValue(BYTE bCardData)
{
	//�˿�����
	BYTE bCardColor=GetCardColor(bCardData);
	BYTE bCardValue=GetCardValue(bCardData);

	//ת����ֵ
	return (bCardValue<=2)?(bCardValue+13):bCardValue;
}

//�Ա��˿�
bool PDKGameLogic::CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount)
{
	//��ȡ����
	BYTE bNextType=GetCardType(bNextList,bNextCount);
	BYTE bFirstType=GetCardType(bFirstList,bFirstCount);

	//�����ж�
	if (bFirstType==CT_ERROR) return false;

	//ը���ж�
	if ((bFirstType==CT_BOMB)&&(bNextType!=CT_BOMB)) return true;
	if ((bFirstType!=CT_BOMB)&&(bNextType==CT_BOMB)) return false;

	//�����ж�
	if ((bFirstType!=bNextType)||(bFirstCount!=bNextCount)) return false;

	//��ʼ�Ա�
	switch (bNextType)
	{
	case CT_BOMB:
	case CT_SINGLE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
		{
			BYTE bNextLogicValue=GetCardLogicValue(bNextList[0]);
			BYTE bFirstLogicValue=GetCardLogicValue(bFirstList[0]);
			return bFirstLogicValue>bNextLogicValue;
		}
	case CT_THREE_LINE:
	case CT_THREE_LINE_TAKE_SINGLE:
	case CT_THREE_LINE_TAKE_DOUBLE:
		{
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(bNextList,bNextCount,NextResult);
			AnalysebCardData(bFirstList,bFirstCount,FirstResult);
			return FirstResult.bThreeLogicVolue[0]>NextResult.bThreeLogicVolue[0];
		}
    case CT_FOUR_LINE_TAKE_THREE:
        {
            tagAnalyseResult NextResult;
            tagAnalyseResult FirstResult;
            AnalysebCardData(bNextList,bNextCount,NextResult);
            AnalysebCardData(bFirstList,bFirstCount,FirstResult);
            return FirstResult.bFourLogicVolue[0]>NextResult.bFourLogicVolue[0];
        }
	}

	return false;
}

bool PDKGameLogic::CompareCard(const std::vector<BYTE>& kFirstList , const std::vector<BYTE>& kNextList)
{
	//��ȡ����
	BYTE bFirstType=GetCardType(kFirstList);
	BYTE bNextType=GetCardType(kNextList);

	BYTE bFirstCount = kFirstList.size();
	BYTE bNextCount = kNextList.size();

	//�����ж�
	if (bFirstType==CT_ERROR) return false;

	//ը���ж�
	if ((bFirstType==CT_BOMB)&&(bNextType!=CT_BOMB)) return true;
    
	if ((bFirstType!=CT_BOMB)&&(bNextType==CT_BOMB)) return false;

	//�����ж�
	if ((bFirstType!=bNextType)||(bFirstCount!=bNextCount)) return false;

	//��ʼ�Ա�
	switch (bNextType)
	{
	case CT_BOMB:
	case CT_SINGLE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
		{
			BYTE bNextLogicValue=GetCardLogicValue(kNextList[0]);
			BYTE bFirstLogicValue=GetCardLogicValue(kFirstList[0]);
			return bFirstLogicValue>bNextLogicValue;
		}
	case CT_THREE_LINE:
	case CT_THREE_LINE_TAKE_SINGLE:
	case CT_THREE_LINE_TAKE_DOUBLE:
		{
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(kNextList,NextResult);
			AnalysebCardData(kFirstList,FirstResult);
            
			return FirstResult.bThreeLogicVolue[0]>NextResult.bThreeLogicVolue[0];
		}
    case CT_FOUR_LINE_TAKE_THREE:
        {
            tagAnalyseResult NextResult;
            tagAnalyseResult FirstResult;
            AnalysebCardData(kNextList,NextResult);
            AnalysebCardData(kFirstList,FirstResult);
            
            return FirstResult.bFourLogicVolue[0]>NextResult.bFourLogicVolue[0];
        }
	}

	return false;
}


//�����˿�
void PDKGameLogic::AnalysebCardData(const BYTE bCardData[], BYTE bCardCount, tagAnalyseResult & AnalyseResult)
{
	//���ý��
	zeromemory(&AnalyseResult,sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i=0;i<bCardCount;i++)
	{
		//��������
		BYTE bSameCount=1;
		BYTE bSameCardData[4]={bCardData[i],0,0,0};
		BYTE bLogicValue=GetCardLogicValue(bCardData[i]);

		//��ȡͬ��
		for (int j=i+1;j<bCardCount;j++)
		{
			//�߼��Ա�
			if (GetCardLogicValue(bCardData[j])!=bLogicValue) break;

			//�����˿�
			bSameCardData[bSameCount++]=bCardData[j];
		}

		//������
		switch (bSameCount)
		{
		case 1:		//����
			{
				AnalyseResult.bSignedLogicVolue[AnalyseResult.bSignedCount]=bLogicValue;
				memcpy(&AnalyseResult.bSignedCardData[(AnalyseResult.bSignedCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 2:		//����
			{
				AnalyseResult.bDoubleLogicVolue[AnalyseResult.bDoubleCount]=bLogicValue;
				memcpy(&AnalyseResult.bDoubleCardData[(AnalyseResult.bDoubleCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 3:		//����
			{
				AnalyseResult.bThreeLogicVolue[AnalyseResult.bThreeCount]=bLogicValue;
				memcpy(&AnalyseResult.bThreeCardData[(AnalyseResult.bThreeCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 4:		//����
			{
				AnalyseResult.bFourLogicVolue[AnalyseResult.bFourCount]=bLogicValue;
				memcpy(&AnalyseResult.bFourCardData[(AnalyseResult.bFourCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		}

		//���õ���
		i+=bSameCount-1;
	}

	return;
}

//�����˿�
void PDKGameLogic::AnalysebCardData(const std::vector<BYTE>& cbCardList, tagAnalyseResult & AnalyseResult)
{
	//���ý��
	zeromemory(&AnalyseResult,sizeof(AnalyseResult));

	int bCardCount = cbCardList.size();
	//�˿˷���
	for (BYTE i=0;i<bCardCount;i++)
	{
		//��������
		BYTE bSameCount=1;
		BYTE bSameCardData[4]={cbCardList[i],0,0,0};
		BYTE bLogicValue=GetCardLogicValue(cbCardList[i]);

		//��ȡͬ��
		for (int j=i+1;j<bCardCount;j++)
		{
			//�߼��Ա�
			if (GetCardLogicValue(cbCardList[j])!=bLogicValue) break;

			//�����˿�
			bSameCardData[bSameCount++]=cbCardList[j];
		}

		//������
		switch (bSameCount)
		{
		case 1:		//����
			{
				AnalyseResult.bSignedLogicVolue[AnalyseResult.bSignedCount]=bLogicValue;
				memcpy(&AnalyseResult.bSignedCardData[(AnalyseResult.bSignedCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 2:		//����
			{
				AnalyseResult.bDoubleLogicVolue[AnalyseResult.bDoubleCount]=bLogicValue;
				memcpy(&AnalyseResult.bDoubleCardData[(AnalyseResult.bDoubleCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		case 3:		//����
			{
				AnalyseResult.bThreeLogicVolue[AnalyseResult.bThreeCount]=bLogicValue;
                
				memcpy(&AnalyseResult.bThreeCardData[(AnalyseResult.bThreeCount++)*bSameCount],bSameCardData,bSameCount);
                
				break;
			}
		case 4:		//����
			{
				AnalyseResult.bFourLogicVolue[AnalyseResult.bFourCount]=bLogicValue;
				memcpy(&AnalyseResult.bFourCardData[(AnalyseResult.bFourCount++)*bSameCount],bSameCardData,bSameCount);
				break;
			}
		}

		//���õ���
		i+=bSameCount-1;
	}

	return;
}

//�����ж�
bool PDKGameLogic::SearchOutCard(BYTE bCardData[], BYTE bCardCount, BYTE bTurnCardData[], BYTE bTurnCardCount, BYTE bTurnOutType, tagOutCardResult & OutCardResult)
{
	//���ý��
	OutCardResult.cbCardCount=0;

	//��������
	switch (bTurnOutType)
	{
	case CT_ERROR:					//��������
		{
			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(bCardData[bCardCount-1]);

			//�����ж�
			BYTE cbSameCount=1;
			for (BYTE i=1;i<bCardCount;i++)
			{
				if (GetCardLogicValue(bCardData[bCardCount-i-1])==bLogicValue) cbSameCount++;
				else break;
			}

			//��ɴ���
			if (cbSameCount>1)
			{
				OutCardResult.cbCardCount=cbSameCount;
				for (BYTE j=0;j<cbSameCount;j++) OutCardResult.cbResultCard[j]=bCardData[bCardCount-1-j];
				return true;
			}

			//���ƴ���
			OutCardResult.cbCardCount=1;
			OutCardResult.cbResultCard[0]=bCardData[bCardCount-1];

			return true;
		}
	case CT_SINGLE:					//��������
		{
			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);
			//�Աȴ�С
			for (BYTE i=0;i<bCardCount;i++)
			{
				if (GetCardLogicValue(bCardData[bCardCount-i-1])>bLogicValue)
				{
					OutCardResult.cbCardCount=1;
					OutCardResult.cbResultCard[0]=bCardData[bCardCount-i-1];
					return true;
				}
			}

			break;
		}
	case CT_SINGLE_LINE:			//��������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//��������
			for (BYTE i=(bTurnCardCount-1);i<bCardCount;i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue>=15) break;
				if (bHandLogicValue<=bLogicValue) continue;

				//��������
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<bCardCount;j++)
				{
					if ((GetCardLogicValue(bCardData[j])+bLineCount)==bHandLogicValue) 
					{
						//��������
						OutCardResult.cbResultCard[bLineCount++]=bCardData[j];

						//����ж�
						if (bLineCount==bTurnCardCount)
						{
							OutCardResult.cbCardCount=bTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	case CT_DOUBLE_LINE:	//��������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//��������
			for (BYTE i=(bTurnCardCount-1);i<bCardCount;i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnCardCount>2)&&(bHandLogicValue>=15)) break;

				//��������
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-1);j++)
				{
					if (((GetCardLogicValue(bCardData[j])+bLineCount)==bHandLogicValue)
						&&((GetCardLogicValue(bCardData[j+1])+bLineCount)==bHandLogicValue))
					{
						//��������
						OutCardResult.cbResultCard[bLineCount*2]=bCardData[j];
						OutCardResult.cbResultCard[(bLineCount++)*2+1]=bCardData[j+1];

						//����ж�
						if (bLineCount*2==bTurnCardCount)
						{
							OutCardResult.cbCardCount=bTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	case CT_THREE_LINE:				//��������
	case CT_THREE_LINE_TAKE_SINGLE:	//����һ��
	case CT_THREE_LINE_TAKE_DOUBLE:	//����һ��
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=0;
			for (BYTE i=0;i<bTurnCardCount-2;i++)
			{
				bLogicValue=GetCardLogicValue(bTurnCardData[i]);
				if (GetCardLogicValue(bTurnCardData[i+1])!=bLogicValue) continue;
				if (GetCardLogicValue(bTurnCardData[i+2])!=bLogicValue) continue;
				break;
			}

			//������ֵ
			BYTE bTurnLineCount=0;
			if (bTurnOutType==CT_THREE_LINE_TAKE_SINGLE) bTurnLineCount=bTurnCardCount/4;
			else if (bTurnOutType==CT_THREE_LINE_TAKE_DOUBLE) bTurnLineCount=bTurnCardCount/5;
			else bTurnLineCount=bTurnCardCount/3;

			//��������
			for (BYTE i=bTurnLineCount*3-1;i<bCardCount;i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnLineCount>1)&&(bHandLogicValue>=15)) break;

				//��������
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-2);j++)
				{
					//�����ж�
					if ((GetCardLogicValue(bCardData[j])+bLineCount)!=bHandLogicValue) continue;
					if ((GetCardLogicValue(bCardData[j+1])+bLineCount)!=bHandLogicValue) continue;
					if ((GetCardLogicValue(bCardData[j+2])+bLineCount)!=bHandLogicValue) continue;

					//��������
					OutCardResult.cbResultCard[bLineCount*3]=bCardData[j];
					OutCardResult.cbResultCard[bLineCount*3+1]=bCardData[j+1];
					OutCardResult.cbResultCard[(bLineCount++)*3+2]=bCardData[j+2];

					//����ж�
					if (bLineCount==bTurnLineCount)
					{
						//��������
						OutCardResult.cbCardCount=bLineCount*3;

						//�����˿�
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount=bCardCount-OutCardResult.cbCardCount;
						memcpy(bLeftCardData,bCardData,sizeof(BYTE)*bCardCount);
						RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,bLeftCardData,bCardCount);

						//�����˿�
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData,bLeftCount,AnalyseResultLeft);

						//���ƴ���
						if (bTurnOutType==CT_THREE_LINE_TAKE_SINGLE)
						{
							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bSignedCount;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=AnalyseResultLeft.bSignedCount-k-1;
								BYTE bSignedCard=AnalyseResultLeft.bSignedCardData[bIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bSignedCard;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bDoubleCount*2;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=(AnalyseResultLeft.bDoubleCount*2-k-1);
								BYTE bSignedCard=AnalyseResultLeft.bDoubleCardData[bIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bSignedCard;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bThreeCount*3;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=(AnalyseResultLeft.bThreeCount*3-k-1);
								BYTE bSignedCard=AnalyseResultLeft.bThreeCardData[bIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bSignedCard;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bFourCount*4;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=(AnalyseResultLeft.bFourCount*4-k-1);
								BYTE bSignedCard=AnalyseResultLeft.bFourCardData[bIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bSignedCard;
							}
						}

						//���ƴ���
						if (bTurnOutType==CT_THREE_LINE_TAKE_DOUBLE)
						{
							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bDoubleCount;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=(AnalyseResultLeft.bDoubleCount-k-1)*2;
								BYTE bCardData1=AnalyseResultLeft.bDoubleCardData[bIndex];
								BYTE bCardData2=AnalyseResultLeft.bDoubleCardData[bIndex+1];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bCardData1;
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bCardData2;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bThreeCount;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=(AnalyseResultLeft.bThreeCount-k-1)*3;
								BYTE bCardData1=AnalyseResultLeft.bThreeCardData[bIndex];
								BYTE bCardData2=AnalyseResultLeft.bThreeCardData[bIndex+1];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bCardData1;
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bCardData2;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bFourCount;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=(AnalyseResultLeft.bFourCount-k-1)*4;
								BYTE bCardData1=AnalyseResultLeft.bFourCardData[bIndex];
								BYTE bCardData2=AnalyseResultLeft.bFourCardData[bIndex+1];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bCardData1;
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bCardData2;
							}
						}

						//����ж�
						if (OutCardResult.cbCardCount!=bTurnCardCount)
						{
							OutCardResult.cbCardCount=0;
							return false;
						}

						return true;
					}
				}
			}

			break;
		}
	case CT_BOMB:						//ը������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(bTurnCardData[0]);

			//����ը��
			for (BYTE i=3;i<bCardCount;i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;

				//ը���ж�
				BYTE bTempLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

				int j = 0;
				for ( j=1;j<4;j++)
				{
					if (GetCardLogicValue(bCardData[bCardCount+j-i-1])!=bTempLogicValue) break;
				}
				if (j!=4) continue;

				//��ɴ���
				OutCardResult.cbCardCount=bTurnCardCount;
				OutCardResult.cbResultCard[0]=bCardData[bCardCount-i-1];
				OutCardResult.cbResultCard[1]=bCardData[bCardCount-i];
				OutCardResult.cbResultCard[2]=bCardData[bCardCount-i+1];
				OutCardResult.cbResultCard[3]=bCardData[bCardCount-i+2];

				return true;
			}

			return false;
		}
	}

	//ը������
	for (BYTE i=3;i<bCardCount;i++)
	{
		//��ȡ��ֵ
		BYTE bHandLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

		//ը���ж�
		BYTE bTempLogicValue=GetCardLogicValue(bCardData[bCardCount-i-1]);

		int j = 0;
		for (j=1;j<4;j++)
		{
			if (GetCardLogicValue(bCardData[bCardCount+j-i-1])!=bTempLogicValue) break;
		}
		if (j!=4) continue;

		//��ɴ���
		OutCardResult.cbCardCount=4;
		OutCardResult.cbResultCard[0]=bCardData[bCardCount-i-1];
		OutCardResult.cbResultCard[1]=bCardData[bCardCount-i];
		OutCardResult.cbResultCard[2]=bCardData[bCardCount-i+1];
		OutCardResult.cbResultCard[3]=bCardData[bCardCount-i+2];

		return true;
	}

	return false;
}

bool PDKGameLogic::SearchOutCard(const std::vector<BYTE>& cbCardList, const std::vector<BYTE>& cbTurnCardList, BYTE bLastOutType, std::vector<tagOutCardResult> & OutCardResultList)
{
	//���ý��
	
    
    //�����Ƶ�����
	int bCardCount = cbCardList.size();
    //��������
	int bTurnCardCount = cbTurnCardList.size();
    
    //��������
	bool isFound = false;
    switch (bLastOutType)
	{
	case CT_ERROR:					//��������
		{
			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(cbCardList[bCardCount-1]);

			//�����ж�
			BYTE cbSameCount=1;
			for (BYTE i=1;i<bCardCount;i++)
			{
				if (GetCardLogicValue(cbCardList[bCardCount-i-1])==bLogicValue) cbSameCount++;
				else break;
			}

			tagOutCardResult OutCardResult;
			OutCardResult.cbCardCount = 0;

			//��ɴ���
			if (cbSameCount>1)
			{
				OutCardResult.cbCardCount=cbSameCount;
				for (BYTE j=0;j<cbSameCount;j++) OutCardResult.cbResultCard[j]=cbCardList[bCardCount-1-j];
				return true;
			}

			//���ƴ���
			OutCardResult.cbCardType = CT_SINGLE;
			OutCardResult.cbCardCount=1;
			OutCardResult.cbResultCard[0]=cbCardList[bCardCount-1];
			
			return true;
		}
	case CT_SINGLE:					//��������
		{
			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(cbTurnCardList[0]);

			//�����˿�
			tagAnalyseResult AnalyseResultLeft;
			AnalysebCardData(cbCardList,AnalyseResultLeft);

			if (AnalyseResultLeft.bSignedCount>0)
			{
				for (int i=AnalyseResultLeft.bSignedCount-1;i>=0;i--)
				{
					if(GetCardLogicValue(AnalyseResultLeft.bSignedCardData[i])>bLogicValue)
					{
						tagOutCardResult OutCardResult;
						OutCardResult.cbCardCount = 0;

						OutCardResult.cbCardType = CT_SINGLE;
						OutCardResult.cbCardCount=1;
						OutCardResult.cbResultCard[0]=AnalyseResultLeft.bSignedCardData[i];
						OutCardResultList.push_back(OutCardResult);
						isFound =  true;
					}
				}
			}
			if (AnalyseResultLeft.bDoubleCount>0)
			{
				for (int i=AnalyseResultLeft.bDoubleCount-1;i>=0;i--)
				{
					if(GetCardLogicValue(AnalyseResultLeft.bDoubleCardData[i])>bLogicValue)
					{
						tagOutCardResult OutCardResult;
						OutCardResult.cbCardCount = 0;

						OutCardResult.cbCardType = CT_SINGLE;
						OutCardResult.cbCardCount=1;
						OutCardResult.cbResultCard[0]=AnalyseResultLeft.bDoubleCardData[i];
						OutCardResultList.push_back(OutCardResult);
						isFound = true;
					}
				}
			}
			if (AnalyseResultLeft.bThreeCount>0)
			{				
				for (int i=AnalyseResultLeft.bThreeCount-1;i>=0;i--)
				{
					if(GetCardLogicValue(AnalyseResultLeft.bThreeCardData[i])>bLogicValue)
					{
						tagOutCardResult OutCardResult;
						OutCardResult.cbCardCount = 0;

						OutCardResult.cbCardType = CT_SINGLE;
						OutCardResult.cbCardCount=1;
						OutCardResult.cbResultCard[0]=AnalyseResultLeft.bThreeCardData[i];
						OutCardResultList.push_back(OutCardResult);
						isFound = true;
					}
				}
			}
			if (AnalyseResultLeft.bFourCount>0)
			{
                //ը������
                for (BYTE i=3;i<bCardCount;i++)
                {
                    
                    //ը���ж�
                    BYTE bTempLogicValue=GetCardLogicValue(cbCardList[bCardCount-i-1]);
                    
                    int j = 0;
                    for (j=1;j<4;j++)
                    {
                        if (GetCardLogicValue(cbCardList[bCardCount+j-i-1])!=bTempLogicValue) break;
                    }
                    if (j!=4) continue;

					tagOutCardResult OutCardResult;
					OutCardResult.cbCardCount = 0;
                    //��ɴ���
					OutCardResult.cbCardType = CT_BOMB;
                    OutCardResult.cbCardCount=4;
                    OutCardResult.cbResultCard[0]=cbCardList[bCardCount-i-1];
                    OutCardResult.cbResultCard[1]=cbCardList[bCardCount-i];
                    OutCardResult.cbResultCard[2]=cbCardList[bCardCount-i+1];
                    OutCardResult.cbResultCard[3]=cbCardList[bCardCount-i+2];
					OutCardResultList.push_back(OutCardResult);
					isFound = true;
                }
            }
            
            break;
		}
	case CT_SINGLE_LINE:			//��������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;
			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(cbTurnCardList[0]);

			//��������
			for (BYTE i=(bTurnCardCount-1);i<bCardCount;i++)
			{
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(cbCardList[bCardCount-i-1]);
				//�����ж�
				if (bHandLogicValue>=15) break;
				if (bHandLogicValue<=bLogicValue) continue;
				tagOutCardResult OutCardResult;
				OutCardResult.cbCardCount = 0;
				//��������
				BYTE bLineCount=0;
				OutCardResult.cbCardType = CT_SINGLE_LINE;
				for (BYTE j=(bCardCount-i-1);j<bCardCount;j++)
				{
					bool isNeedBreak = false;
					if ((GetCardLogicValue(cbCardList[j])+bLineCount)==bHandLogicValue) 
					{
						//��������
						OutCardResult.cbResultCard[bLineCount++]=cbCardList[j];

						//����ж�
						if (bLineCount==bTurnCardCount)
						{
							OutCardResult.cbCardCount=bTurnCardCount;
							OutCardResultList.push_back(OutCardResult);
							isNeedBreak = true;
							isFound = true;
							break;
							// return true;
						}

						if (isNeedBreak)
						{
							break;
						}
					}
				}
			}

			break;
		}
	case CT_DOUBLE_LINE:	//��������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(cbTurnCardList[0]);

			//��������
			for (BYTE i=(bTurnCardCount-1);i<bCardCount;i++)
			{
				bool isNeedBreak = false;
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(cbCardList[bCardCount-i-1]);
				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnCardCount>2)&&(bHandLogicValue>=15)) break;

				//��������
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-1);j++)
				{
					tagOutCardResult OutCardResult;
					OutCardResult.cbCardType = CT_DOUBLE_LINE;
					OutCardResult.cbCardCount = 0;
					if (((GetCardLogicValue(cbCardList[j])+bLineCount)==bHandLogicValue)
						&&((GetCardLogicValue(cbCardList[j+1])+bLineCount)==bHandLogicValue))
					{
						//��������
						OutCardResult.cbResultCard[bLineCount*2]=cbCardList[j];
						OutCardResult.cbResultCard[(bLineCount++)*2+1]=cbCardList[j+1];

						//����ж�
						if (bLineCount*2==bTurnCardCount)
						{
							OutCardResult.cbCardCount=bTurnCardCount;
							isFound = true;
							OutCardResultList.push_back(OutCardResult);
						}
					}
				}
			}

			break;
		}
	case CT_THREE_LINE_TAKE_DOUBLE:	//����һ��
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=0;
			for (BYTE i=0;i<bTurnCardCount-2;i++)
			{
				bLogicValue=GetCardLogicValue(cbTurnCardList[i]);
				if (GetCardLogicValue(cbTurnCardList[i+1])!=bLogicValue) continue;
				if (GetCardLogicValue(cbTurnCardList[i+2])!=bLogicValue) continue;
				break;
			}

			//������ֵ
			BYTE bTurnLineCount=0;
			if (bLastOutType==CT_THREE_LINE_TAKE_SINGLE)
                bTurnLineCount=bTurnCardCount/4;
			else if (bLastOutType==CT_THREE_LINE_TAKE_DOUBLE)
                bTurnLineCount=bTurnCardCount/5;
			else
                bTurnLineCount=bTurnCardCount/3;

			//��������
			for (BYTE i=bTurnLineCount*3-2;i<bCardCount;i++)
			{

				tagOutCardResult OutCardResult;
				OutCardResult.cbCardCount = 0;

				bool isNeedBreak = false;
				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(cbCardList[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;
				if ((bTurnLineCount>1)&&(bHandLogicValue>=15)) break;

				//��������
				BYTE bLineCount=0;
				for (BYTE j=(bCardCount-i-1);j<(bCardCount-2);j++)
				{
					//�����ж�
					if ((GetCardLogicValue(cbCardList[j])+bLineCount)!=bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardList[j+1])+bLineCount)!=bHandLogicValue) continue;
					if ((GetCardLogicValue(cbCardList[j+2])+bLineCount)!=bHandLogicValue) continue;

					//��������
					OutCardResult.cbResultCard[bLineCount*3]=cbCardList[j];
					OutCardResult.cbResultCard[bLineCount*3+1]=cbCardList[j+1];
					OutCardResult.cbResultCard[(bLineCount++)*3+2]=cbCardList[j+2];

					//����ж�
					if (bLineCount==bTurnLineCount)
					{
						//��������
						OutCardResult.cbCardCount=bLineCount*3;

						//�����˿�
						BYTE bLeftCardData[MAX_COUNT];
						BYTE bLeftCount=bCardCount-OutCardResult.cbCardCount;

						for (int i=0;i<bCardCount;i++)
						{
							bLeftCardData[i] = cbCardList[i];
						}
						RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,bLeftCardData,bCardCount);

						//�����˿�
						tagAnalyseResult AnalyseResultLeft;
						AnalysebCardData(bLeftCardData,bLeftCount,AnalyseResultLeft);

//						//���ƴ���
						if (bLastOutType==CT_THREE_LINE_TAKE_DOUBLE)
						{
							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bSignedCount;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=AnalyseResultLeft.bSignedCount-k-1;
								BYTE bSignedCard=AnalyseResultLeft.bSignedCardData[bIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bSignedCard;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bDoubleCount*2;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=(AnalyseResultLeft.bDoubleCount*2-k-1);
								BYTE bSignedCard=AnalyseResultLeft.bDoubleCardData[bIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bSignedCard;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bThreeCount*3;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=(AnalyseResultLeft.bThreeCount*3-k-1);
								BYTE bSignedCard=AnalyseResultLeft.bThreeCardData[bIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bSignedCard;
							}

							//��ȡ����
							for (BYTE k=0;k<AnalyseResultLeft.bFourCount*4;k++)
							{
								//��ֹ�ж�
								if (OutCardResult.cbCardCount==bTurnCardCount) break;

								//�����˿�
								BYTE bIndex=(AnalyseResultLeft.bFourCount*4-k-1);
								BYTE bSignedCard=AnalyseResultLeft.bFourCardData[bIndex];
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=bSignedCard;
							}
						}

						//����ж�
						if (OutCardResult.cbCardCount!=bTurnCardCount)
						{
							OutCardResult.cbCardCount=0;
							isNeedBreak = true;
							break;
						}

						isFound = true;
						
						OutCardResultList.push_back(OutCardResult);
						
					}

					if (isNeedBreak)
					{
						break;
					}
				}
			}


			break;
		}
	case CT_BOMB:						//ը������
		{
			//�����ж�
			if (bCardCount<bTurnCardCount) break;

			//��ȡ��ֵ
			BYTE bLogicValue=GetCardLogicValue(cbTurnCardList[0]);

			bool isRealFound = false;
			//����ը��
			for (BYTE i=3;i<bCardCount;i++)
			{
				tagOutCardResult OutCardResult;
				OutCardResult.cbCardCount = 0;


				//��ȡ��ֵ
				BYTE bHandLogicValue=GetCardLogicValue(cbCardList[bCardCount-i-1]);

				//�����ж�
				if (bHandLogicValue<=bLogicValue) continue;

				//ը���ж�
				BYTE bTempLogicValue=GetCardLogicValue(cbCardList[bCardCount-i-1]);

				int j = 0;
				for ( j=1;j<4;j++)
				{
					if (GetCardLogicValue(cbCardList[bCardCount+j-i-1])!=bTempLogicValue) break;
				}
				if (j!=4) continue;

				//��ɴ���
				OutCardResult.cbCardType = CT_BOMB;
				OutCardResult.cbCardCount=bTurnCardCount;
				OutCardResult.cbResultCard[0]=cbCardList[bCardCount-i-1];
				OutCardResult.cbResultCard[1]=cbCardList[bCardCount-i];
				OutCardResult.cbResultCard[2]=cbCardList[bCardCount-i+1];
				OutCardResult.cbResultCard[3]=cbCardList[bCardCount-i+2];

				OutCardResultList.push_back(OutCardResult);
				isRealFound = true;
			}

			return isRealFound;
		}
	}

    //ը������
	bool isFoundBom = false;
    for (BYTE i=3;i<bCardCount;i++)
    {

		tagOutCardResult OutCardResult;
		OutCardResult.cbCardCount = 0;

        //ը���ж�
        BYTE bTempLogicValue=GetCardLogicValue(cbCardList[bCardCount-i-1]);
        
        int j = 0;
        for (j=1;j<4;j++)
        {
            if (GetCardLogicValue(cbCardList[bCardCount+j-i-1])!=bTempLogicValue) break;
        }
        if (j!=4) continue;
        
        //��ɴ���
		OutCardResult.cbCardType = CT_BOMB;
        OutCardResult.cbCardCount=4;
        OutCardResult.cbResultCard[0]=cbCardList[bCardCount-i-1];
        OutCardResult.cbResultCard[1]=cbCardList[bCardCount-i];
        OutCardResult.cbResultCard[2]=cbCardList[bCardCount-i+1];
        OutCardResult.cbResultCard[3]=cbCardList[bCardCount-i+2];
		isFoundBom = true;
		OutCardResultList.push_back(OutCardResult);
    }
    
	if (isFound)
	{
		return true;
	}

	return isFoundBom;
}

//////////////////////////////////////////////////////////////////////////