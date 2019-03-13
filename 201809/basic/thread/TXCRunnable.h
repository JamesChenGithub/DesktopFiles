//
// Created by alderzhang on 2017/6/23.
//

#ifndef TXCRUNNABLE_H
#define TXCRUNNABLE_H

struct TXCRunnable {
	virtual ~TXCRunnable() {}

	virtual void run() = 0;
};

template<class T>
class TXCRunnableFunctor : public TXCRunnable {
public:
	TXCRunnableFunctor(const T &f) : func_(f) {}

	virtual void run() { func_(); }

private:
	T func_;
};

template<class T>
class TXCRunnableFunctor<T *> : public TXCRunnable {
public:
	TXCRunnableFunctor(T *f) : func_(f) {}

	virtual void run() { (*func_)(); }

private:
	TXCRunnableFunctor(const TXCRunnableFunctor &);

	TXCRunnableFunctor &operator=(const TXCRunnableFunctor &);

private:
	T *func_;
};

template<>
class TXCRunnableFunctor<TXCRunnable> : public TXCRunnable {
	TXCRunnableFunctor();
};

template<>
class TXCRunnableFunctor<TXCRunnable *> : public TXCRunnable {
public:
	TXCRunnableFunctor(TXCRunnable *f) : func_(f) {}

	virtual void run() { static_cast<TXCRunnable *>(func_)->run(); }

private:
	TXCRunnableFunctor(const TXCRunnableFunctor &);

	TXCRunnableFunctor &operator=(const TXCRunnableFunctor &);

private:
	TXCRunnable *func_;
};

// base template for no argument functor
template<class T>
struct TXSTransformImplement {
	static TXCRunnable *transform(const T &t) {
		return new TXCRunnableFunctor<T>(t);
	}
};

template<class T>
inline TXCRunnable *txf_runnable_transform(const T &t) {
	return TXSTransformImplement<T>::transform(t);
}

#endif /* TXCRUNNABLE_H */
