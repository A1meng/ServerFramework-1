#ifndef __TIME_STAMP_CONVERTER_HPP__
#define __TIME_STAMP_CONVERTER_HPP__

// TimeStamp�ĸ�16��bit���session fd����16��bit��Ÿ�����value
unsigned int GenerateTimeStamp(const unsigned int uiSessionFD, const unsigned short unValue);
// ��TimeStamp�н�����session fd
unsigned int TimeStampToSessionFD(const unsigned int uiTimeStamp);
// ��TimeStamp�н���������value
unsigned short TimeStampToValue(const unsigned int uiTimeStamp);

#endif // __TIME_STAMP_CONVERTER_HPP__
