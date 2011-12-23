#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

template <typename T>

class Singleton
{
	protected:
	  // Constructeur/destructeur
	  Singleton () { }
	  ~Singleton () { }

	public:
	  // Interface publique
	  static T *getInstance ()
	  {
		if (NULL == _inst) _inst =  new T;
		  
		return (static_cast<T*> (_inst));
	  }

	  static void kill ()
	  {
		if (NULL != _inst)
		  {
			delete _inst;
			_inst = NULL;
		  }
	  }

	private:
	  // Unique instance
	  static T *_inst;
};

template <typename T>
T *Singleton<T>::_inst = NULL;

#endif
