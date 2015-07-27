#ifndef TRANS_H_
#define TRANS_H_ 
class Trans
{
public:
	Trans(int, int);
	~Trans(void);
	void update(float);
	void reduceBand(int);
	void finish();
	bool ifActive();
	void updateBand(int);
private:
	bool active;
	int dataSize;
	int availBand;
	int remainningDataSize;
};

#endif