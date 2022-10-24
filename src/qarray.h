#ifndef QARRAY_H
#define QARRAY_H

#include <qvector.h>


template <typename T> class QArray1 : public QVector<T> {
 public:
  QArray1<T>(int len) : QVector<T>(len) {};
  T &operator() (int i);
  QArray1<T> &operator= (const T &value);
  QArray1<T> &operator= (const QArray1<T> &other);
};

template <typename T> class QArray2 : public QVector<QVector<T> > {
 public:
  QArray2<T>(int ilen, int jlen);
  T &operator() (int i, int j);
};

typedef QArray1<double> QDVector;
typedef QArray1<int> QIVector;
typedef QArray2<double> QDMatrix;
typedef QArray2<int> QIMatrix;

//----------------------------------------------------------------------------------------------------
// 1-D arrays

template <typename T>
T &QArray1<T>::operator() (int i) {
  if (i < 1 || i > this->size()) {
    printf("operator() :: Attempting to read/write outside an array.\n");
    exit(0);
  }
  T *data = this->data();
  return data[i-1];
}

template <typename T>
QArray1<T> &QArray1<T>::operator= (const T &value) {
  std::fill(this->data(), this->data()+sizeof(T)*this->size(), value);
  return *this;
}

template <typename T>
QArray1<T> &QArray1<T>::operator= (const QArray1<T> &other) {
  if (this->size() != other.size()) {
    printf("operator= :: Cannot add vectors of different sizes.\n");
    printf("%i %i\n", this->size(), other.size());
    exit(0);
  }
  memcpy(this->data(), other.data(), sizeof(T)*this->size());
  return *this;
}

//----------------------------------------------------------------------------------------------------
// 2-D arrays
  
template <typename T>
QArray2<T>::QArray2(int ilen, int jlen) : QVector<QVector<T> >(ilen) {
  QVector<T> *ivec = this->data();
  for (int i=0; i<ilen; i++) {    
    ivec[i].resize(jlen);
  }
}

template <typename T>
T &QArray2<T>::operator() (int i, int j) {
  QVector<T> *ivec = this->data();
  T *jvec = ivec[i-1].data();
  return jvec[j-1];
}

//----------------------------------------------------------------------------------------------------


#endif

