#pragma once
#ifndef __IMAGE_H_
#define __IMAGE_H_
#include <vector>
#ifndef min
#define min
#endif
#ifndef max
#define max
#endif
#include <atlimage.h>
struct Image
{
	Image() :width(0), height(0), pdata(nullptr) {
	}
	Image(int w, int h) :pdata(nullptr) {
		
		create(w, h);
	}
	Image(const Image& rhs) :pdata(nullptr) {
		
		if (rhs.empty()) {
			this->clear();
		}
		else {
			this->create(rhs.width, rhs.height);
			memcpy(this->pdata, rhs.pdata, width*height * 4);
		}

	}
	~Image() {
		release();
	}
	void create(int w, int h) {
		width = w, height = h;
		if (!pdata) {
			pdata = (unsigned char*)malloc(w*h * 4);
		}
		else {
			pdata = (unsigned char*)realloc(pdata, w*h * 4);
		}
		if (pdata == nullptr)throw("memory not enough");
	}
	void release() {
		width = height = 0;
		if (pdata)free(pdata);
		pdata = nullptr;
	}
	void clear() {
		width = height = 0;
	}

	bool empty()const {
		return width == 0;
	}

	Image& operator=(const Image& rhs) {
		if (rhs.empty()) {
			this->clear();
		}
		else if (this != &rhs) {
			this->create(rhs.width, rhs.height);
			memcpy(this->pdata, rhs.pdata, width*height * 4);
		}
		return *this;
	}
	bool read(LPCTSTR file) {
		clear();
		ATL::CImage img;
		HRESULT hr = img.Load(file);
		if (hr == S_OK) {
			create(img.GetWidth(), img.GetHeight());
			translate((unsigned char*)img.GetBits(), img.GetBPP() / 8, img.GetPitch());
			return true;
		}
		else {
			return false;
		}
	}
	bool write(LPCTSTR file) {
		if (empty())
			return false;
		ATL::CImage img;
		
		img.Create(width, height, 32);
		auto pdst = (unsigned char*)img.GetBits();
		auto psrc = pdata;
		int pitch = img.GetPitch();
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				((int*)pdst)[j] = ((int*)psrc)[j];
			}
			pdst += pitch;
			psrc += width * 4;
		}
		return img.Save(file) == S_OK;
	}
	void translate(unsigned char* psrc, int pixSize, int pitch) {
		auto pdst = pdata;
		//gray
		if (pixSize == 1) {
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					*pdst++ = psrc[j];
					*pdst++ = psrc[j];
					*pdst++ = psrc[j];
					*pdst++ = 0;
				}
				psrc += pitch;

			}
		}//bgr
		else if (pixSize == 3) {
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					*pdst++ = psrc[j * 3 + 0];
					*pdst++ = psrc[j * 3 + 1];
					*pdst++ = psrc[j * 3 + 2];
					*pdst++ = 0;
				}
				psrc += pitch;

			}
		}
		else if (pixSize == 4) {
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					*pdst++ = psrc[j * 4 + 0];
					*pdst++ = psrc[j * 4 + 1];
					*pdst++ = psrc[j * 4 + 2];
					*pdst++ = psrc[j * 4 + 3];
				}
				psrc += pitch;

			}
		}
	}
	template<typename Tp>
	Tp at(int y, int x)const {
		return ((Tp*)pdata)[y*width + x];
	}
	template<typename Tp>
	Tp& at(int y, int x) {
		return ((Tp*)pdata)[y*width + x];
	}
	template<typename Tp>
	Tp* ptr(int y) {
		return (Tp*)(pdata + y * width * 4);
	}


	int width, height;
	unsigned char* pdata;
};
//��ͨ��ͼ��
struct ImageBin {
	ImageBin() :width(0), height(0) {}
	ImageBin(const ImageBin& rhs) {
		this->width = rhs.width;
		this->height = rhs.height;
		this->pixels = rhs.pixels;
	}
	void create(int w, int h) {
		width = w, height = h;
		pixels.resize(w*h);
	}
	void clear() {
		width = height = 0;
	}
	bool empty()const {
		return width == 0;
	}
	unsigned char* data() {
		return pixels.data();
	}
	ImageBin& operator=(const ImageBin& rhs) {
		this->width = rhs.width;
		this->height = rhs.height;
		this->pixels = rhs.pixels;
		return *this;
	}
	unsigned char at(int y,int x)const {
		return pixels[y*width + x];
	}
	unsigned char& at(int y, int x) {
		return pixels[y*width + x];
	}

	unsigned char* ptr(int y) {
		return pixels.data() + y * width;
	}

	void fromImage4(const Image& img4) {
		create(img4.width, img4.height);
		auto psrc = img4.pdata;
		for (int i = 0; i < pixels.size(); ++i) {
			pixels[i] = (psrc[0] + psrc[1] + psrc[2]) / 3;
			psrc += 4;
		}
	}

	int width, height;
	std::vector<unsigned char> pixels;
};

#endif