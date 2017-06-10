//-----------------------------------------------------
//
// Code generated with Faust 0.9.71 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with  */
/************************************************************************

	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2004-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either version 3
	of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.

	MAX MSP SDK : in order to compile a MaxMSP external with this
	architecture file you will need the official MaxMSP SDK from
	cycling'74. Please check the corresponding license.

 ************************************************************************
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __APPLE__
#include <Carbon/Carbon.h>
#include <unistd.h>
#endif

#define FAUSTFLOAT double

#ifndef FAUST_UI_H
#define FAUST_UI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust User Interface
 * This abstract class contains only the method that the faust compiler can
 * generate to describe a DSP interface.
 ******************************************************************************/

class UI
{

 public:

	UI() {}

	virtual ~UI() {}

    // -- widget's layouts

    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;

    // -- active widgets

    virtual void addButton(const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;

    // -- passive widgets

    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;

	// -- metadata declarations

    virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
};

#endif
#ifndef FAUST_JSONUI_H
#define FAUST_JSONUI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#ifndef FAUST_PathUI_H
#define FAUST_PathUI_H

#include <vector>
#include <string>
#include <algorithm>

/*******************************************************************************
 * PathUI : Faust User Interface
 * Helper class to build complete path for items.
 ******************************************************************************/

class PathUI : public UI
{

    protected:
    
        std::vector<std::string> fControlsLevel;
       
    public:
    
        std::string buildPath(const std::string& label) 
        {
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res += fControlsLevel[i];
                res += "/";
            }
            res += label;
            replace(res.begin(), res.end(), ' ', '_');
            return res;
        }
    
};

#endif
/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

 ************************************************************************
 ************************************************************************/
 
#ifndef __meta__
#define __meta__

struct Meta
{
    virtual void declare(const char* key, const char* value) = 0;
};

#endif


#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include <assert.h>

/*******************************************************************************
 * JSONUI : Faust User Interface
 * This class produce a complete JSON decription of the DSP instance.
 ******************************************************************************/

class JSONUI : public PathUI, public Meta
{

    protected:
    
        std::stringstream fJSON;
        std::stringstream fUI;
        std::stringstream fMeta;
        std::vector<std::pair <std::string, std::string> > fMetaAux;
        std::string fName;
    
        char fCloseUIPar;
        char fCloseMetaPar;
        int fTab;
    
        int fInputs, fOutputs;
         
        void tab(int n, std::ostream& fout)
        {
            fout << '\n';
            while (n-- > 0) {
                fout << '\t';
            }
        }
    
        void addMeta(int tab_val, bool quote = true)
        {
            if (fMetaAux.size() > 0) {
                tab(tab_val, fUI); fUI << "\"meta\": [";
                std::string sep = "";
                for (size_t i = 0; i < fMetaAux.size(); i++) {
                    fUI << sep;
                    tab(tab_val + 1, fUI); fUI << "{ \"" << fMetaAux[i].first << "\": \"" << fMetaAux[i].second << "\" }";
                    sep = ",";
                }
                tab(tab_val, fUI); fUI << ((quote) ? "],": "]");
                fMetaAux.clear();
            }
        }
        
        void init(const std::string& name, int inputs, int outputs)
        {
            fTab = 1;
            
            // Start Meta generation
            tab(fTab, fMeta); fMeta << "\"meta\": [";
            fCloseMetaPar = ' ';
            
            // Start UI generation
            tab(fTab, fUI); fUI << "\"ui\": [";
            fCloseUIPar = ' ';
            fTab += 1;
            
            fName = name;
            fInputs = inputs;
            fOutputs = outputs;
        }
      
     public:
     
        JSONUI(const std::string& name, int inputs, int outputs)
        {
            init(name, inputs, outputs);
        }

        JSONUI(int inputs, int outputs)
        {
            init("", inputs, outputs);
        }
 
        virtual ~JSONUI() {}

        // -- widget's layouts
    
        virtual void openGenericGroup(const char* label, const char* name)
        {
            fControlsLevel.push_back(label);
            fUI << fCloseUIPar;
            tab(fTab, fUI); fUI << "{";
            fTab += 1;
            tab(fTab, fUI); fUI << "\"type\": \"" << name << "\",";
            tab(fTab, fUI); fUI << "\"label\": \"" << label << "\",";
            addMeta(fTab + 1);
            tab(fTab, fUI); fUI << "\"items\": [";
            fCloseUIPar = ' ';
            fTab += 1;
        }

        virtual void openTabBox(const char* label)
        {
            openGenericGroup(label, "tgroup");
        }
    
        virtual void openHorizontalBox(const char* label)
        {
            openGenericGroup(label, "hgroup");
        }
    
        virtual void openVerticalBox(const char* label)
        {
            openGenericGroup(label, "vgroup");
        }
    
        virtual void closeBox()
        {
            fControlsLevel.pop_back();
            fTab -= 1;
            tab(fTab, fUI); fUI << "]";
            fTab -= 1;
            tab(fTab, fUI); fUI << "}";
            fCloseUIPar = ',';
        }
    
        // -- active widgets
    
        virtual void addGenericButton(const char* label, const char* name)
        {
            fUI << fCloseUIPar;
            tab(fTab, fUI); fUI << "{";
            tab(fTab + 1, fUI); fUI << "\"type\": \"" << name << "\",";
            tab(fTab + 1, fUI); fUI << "\"label\": \"" << label << "\"" << ",";
            tab(fTab + 1, fUI); fUI << "\"address\": \"" << buildPath(label) << "\"" << ((fMetaAux.size() > 0) ? "," : "");
            addMeta(fTab + 1, false);
            tab(fTab, fUI); fUI << "}";
            fCloseUIPar = ',';
        }

        virtual void addButton(const char* label, FAUSTFLOAT* zone)
        {
            addGenericButton(label, "button");
        }
    
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            addGenericButton(label, "checkbox");
        }

        virtual void addGenericEntry(const char* label, const char* name, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            fUI << fCloseUIPar;
            tab(fTab, fUI); fUI << "{";
            tab(fTab + 1, fUI); fUI << "\"type\": \"" << name << "\",";
            tab(fTab + 1, fUI); fUI << "\"label\": \"" << label << "\"" << ",";
            tab(fTab + 1, fUI); fUI << "\"address\": \"" << buildPath(label) << "\"" << ",";
            addMeta(fTab + 1);
            tab(fTab + 1, fUI); fUI << "\"init\": \"" << init << "\",";
            tab(fTab + 1, fUI); fUI << "\"min\": \"" << min << "\",";
            tab(fTab + 1, fUI); fUI << "\"max\": \"" << max << "\",";
            tab(fTab + 1, fUI); fUI << "\"step\": \"" << step << "\"";
            tab(fTab, fUI); fUI << "}";
            fCloseUIPar = ',';
        }
    
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addGenericEntry(label, "vslider", init, min, max, step);
        }
    
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addGenericEntry(label, "hslider", init, min, max, step);
        }
    
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addGenericEntry(label, "nentry", init, min, max, step);
        }

        // -- passive widgets
    
        virtual void addGenericBargraph(const char* label, const char* name, FAUSTFLOAT min, FAUSTFLOAT max) 
        {
            fUI << fCloseUIPar;
            tab(fTab, fUI); fUI << "{";
            tab(fTab + 1, fUI); fUI << "\"type\": \"" << name << "\",";
            tab(fTab + 1, fUI); fUI << "\"label\": \"" << label << "\"" << ",";
            tab(fTab + 1, fUI); fUI << "\"address\": \"" << buildPath(label) << "\"" << ",";
            addMeta(fTab + 1);
            tab(fTab + 1, fUI); fUI << "\"min\": \"" << min << "\",";
            tab(fTab + 1, fUI); fUI << "\"max\": \"" << max << "\"";
            tab(fTab, fUI); fUI << "}";
            fCloseUIPar = ',';
        }

        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 
        {
            addGenericBargraph(label, "hbargraph", min, max);
        }
    
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
        {
            addGenericBargraph(label, "vbargraph", min, max);
        }

        // -- metadata declarations

        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {
            fMetaAux.push_back(std::make_pair(key, val));
        }
    
        // Meta interface
        virtual void declare(const char* key, const char* value)
        {
            fMeta << fCloseMetaPar;
            if ((strcmp(key, "name") == 0) && (fName == "")) fName = value;
            tab(fTab, fMeta); fMeta << "{ " << "\"" << key << "\"" << ": " << "\"" << value << "\" }";
            fCloseMetaPar = ',';
        }
    
        inline std::string flatten(const std::string& src)
        {
            std::stringstream dst;
            for (size_t i = 0; i < src.size(); i++) {
                switch (src[i]) {
                    case '\n':
                    case '\t':
                        dst << ' ';
                        break;
                    case '"':
                        dst << "\\" << '"';
                        break;
                    default:
                        dst << src[i];
                        break;
                }
            }
            return dst.str();
        }
    
        std::string JSON(bool flat = false)
        {
            fTab = 0;
            fJSON << "{";
            fTab += 1;
            tab(fTab, fJSON); fJSON << "\"name\": \"" << fName << "\",";
            if (fInputs > 0) { tab(fTab, fJSON); fJSON << "\"inputs\": \"" << fInputs << "\","; }
            if (fOutputs > 0) { tab(fTab, fJSON); fJSON << "\"outputs\": \"" << fOutputs << "\","; }
            tab(fTab, fMeta); fMeta << "],";
            tab(fTab, fUI); fUI << "]";
            fTab -= 1;
            if (fCloseMetaPar == ',') { // If "declare" has been called, fCloseMetaPar state is now ','
                fJSON << fMeta.str() << fUI.str();
            } else {
                fJSON << fUI.str();
            }
            tab(fTab, fJSON); fJSON << "}" << std::endl;
            return (flat) ? flatten(fJSON.str()) : fJSON.str();
        }
    
};

#endif
/************************************************************************
	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/
 
/******************************************************************************
*******************************************************************************

								FAUST DSP

*******************************************************************************
*******************************************************************************/

#ifndef __dsp__
#define __dsp__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class UI;

//----------------------------------------------------------------
//  signal processor definition
//----------------------------------------------------------------

class dsp {
 protected:
	int fSamplingFreq;
 public:
	dsp() {}
	virtual ~dsp() {}

	virtual int getNumInputs() 										= 0;
	virtual int getNumOutputs() 									= 0;
	virtual void buildUserInterface(UI* ui_interface) 				= 0;
	virtual void init(int samplingRate) 							= 0;
 	virtual void compute(int len, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) 	= 0;
};

// On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
// flags to avoid costly denormals
#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

 ************************************************************************
 ************************************************************************/
 
#ifndef __misc__
#define __misc__

#include <algorithm>
#include <map>
#include <string.h>
#include <stdlib.h>


using std::max;
using std::min;

struct XXXX_Meta : std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

struct MY_Meta : Meta, std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

inline int	lsr(int x, int n)	{ return int(((unsigned int)x) >> n); }
inline int 	int2pow2(int x)		{ int r=0; while ((1<<r)<x) r++; return r; }

long lopt(char *argv[], const char *name, long def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return atoi(argv[i+1]);
	return def;
}

bool isopt(char *argv[], const char *name)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return true;
	return false;
}

const char* lopts(char *argv[], const char *name, const char* def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return argv[i+1];
	return def;
}
#endif


using namespace std;

/******************************************************************************
*******************************************************************************

							       VECTOR INTRINSICS

*******************************************************************************
*******************************************************************************/


/********************END ARCHITECTURE SECTION (part 1/2)****************/

/**************************BEGIN USER SECTION **************************/

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
  private:
  public:
	static void metadata(Meta* m) 	{ 
	}

	virtual int getNumInputs() 	{ return 7; }
	virtual int getNumOutputs() 	{ return 7; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("0x00");
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* input2 = input[2];
		FAUSTFLOAT* input3 = input[3];
		FAUSTFLOAT* input4 = input[4];
		FAUSTFLOAT* input5 = input[5];
		FAUSTFLOAT* input6 = input[6];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		FAUSTFLOAT* output2 = output[2];
		FAUSTFLOAT* output3 = output[3];
		FAUSTFLOAT* output4 = output[4];
		FAUSTFLOAT* output5 = output[5];
		FAUSTFLOAT* output6 = output[6];
		for (int i=0; i<count; i++) {
			output0[i] = (FAUSTFLOAT)(double)input0[i];
			double fTemp0 = (double)input2[i];
			double fTemp1 = (double)input1[i];
			output1[i] = (FAUSTFLOAT)((0.8939966636005579 * fTemp0) - (0.4480736161291701 * fTemp1));
			output2[i] = (FAUSTFLOAT)((0.8939966636005579 * fTemp1) - (0.4480736161291701 * fTemp0));
			double fTemp2 = (double)input3[i];
			double fTemp3 = (double)input4[i];
			output3[i] = (FAUSTFLOAT)(0 - ((0.5984600690578581 * fTemp2) + (0.8011526357338306 * fTemp3)));
			output4[i] = (FAUSTFLOAT)(0 - ((0.5984600690578581 * fTemp3) + (0.8011526357338306 * fTemp2)));
			double fTemp4 = (double)input5[i];
			double fTemp5 = (double)input6[i];
			output5[i] = (FAUSTFLOAT)((0.9843819506325049 * fTemp4) - (0.17604594647121138 * fTemp5));
			output6[i] = (FAUSTFLOAT)((0.9843819506325049 * fTemp5) - (0.17604594647121138 * fTemp4));
		}
	}
};



/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/


/* Faust code wrapper ------- */

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "jpatcher_api.h"
#include <string.h>

#define ASSIST_INLET 	1  		/* should be defined somewhere ?? */
#define ASSIST_OUTLET 	2		/* should be defined somewhere ?? */

#define EXTERNAL_VERSION "0.55"

class mspUI;

struct Max_Meta1 : Meta
{
    int fCount;
    
    Max_Meta1():fCount(0)
    {}
     
    void declare(const char* key, const char* value)
    {
        if ((strcmp("name", key) == 0) || (strcmp("author", key) == 0)) {
            fCount++;
        }
    }
};

struct Max_Meta2 : Meta
{
    void declare(const char* key, const char* value)
    {
        if ((strcmp("name", key) == 0) || (strcmp("author", key) == 0)) {
            post("%s : %s", key, value);
        }
    }
};


/*--------------------------------------------------------------------------*/
typedef struct faust
{
    t_pxobject m_ob;
    t_atom *m_seen, *m_want;
    map<string, t_object*> m_output_table;
    short m_where;
    bool m_mute;
    void** args;
    mspUI* dspUI;
    mydsp* dsp;
    char* m_json;
} t_faust;

void *faust_class;

/*--------------------------------------------------------------------------*/
class mspUIObject {

	protected:

		string fLabel;
		FAUSTFLOAT* fZone;

		FAUSTFLOAT range(FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT val) {return (val < min) ? min : (val > max) ? max : val;}

	public:

		mspUIObject(const string& label, FAUSTFLOAT* zone):fLabel(label),fZone(zone) {}
		virtual ~mspUIObject() {}

		virtual void setValue(FAUSTFLOAT f) {*fZone = range(0.0,1.0,f);}
        virtual FAUSTFLOAT getValue() { return *fZone; }
		virtual void toString(char* buffer) {}
		virtual string getName() {return fLabel;}
};

/*--------------------------------------------------------------------------*/
class mspCheckButton : public mspUIObject {

	public:

		mspCheckButton(const string& label, FAUSTFLOAT* zone):mspUIObject(label,zone) {}
		virtual ~mspCheckButton() {}

		void toString(char* buffer)
		{
            sprintf(buffer, "CheckButton(double): %s", fLabel.c_str());
		}
};

/*--------------------------------------------------------------------------*/
class mspButton : public mspUIObject {

	public:

		mspButton(const string& label, FAUSTFLOAT* zone):mspUIObject(label,zone) {}
		virtual ~mspButton() {}

		void toString(char* buffer)
		{
            sprintf(buffer, "Button(double): %s", fLabel.c_str());
		}
};

/*--------------------------------------------------------------------------*/
class mspSlider : public mspUIObject {

	private:

		FAUSTFLOAT fInit;
		FAUSTFLOAT fMin;
		FAUSTFLOAT fMax;
		FAUSTFLOAT fStep;

	public:

		mspSlider(const string& label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
			:mspUIObject(label,zone),fInit(init),fMin(min),fMax(max),fStep(step) {}
		virtual ~mspSlider() {}

		void toString(char* buffer)
		{
            stringstream s; 
            s << "Slider(double): " << fLabel << " [init=" << fInit << ":min=" << fMin << ":max=" << fMax << ":step=" << fStep << ":cur=" << *fZone << "]";
            strcpy(buffer, s.str().c_str());
		}

		void setValue(FAUSTFLOAT f) {*fZone = range(fMin,fMax,f);}
};

/*--------------------------------------------------------------------------*/
class mspBargraph : public mspUIObject {
    
    private:
        
        FAUSTFLOAT fMin;
        FAUSTFLOAT fMax;
        FAUSTFLOAT fCurrent;
        
    public:
        
        mspBargraph(const string& label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
        :mspUIObject(label,zone),fMin(min),fMax(max),fCurrent(*zone) {}
        virtual ~mspBargraph() {}
        
        void toString(char* buffer)
        {
            stringstream s; 
            s << "Bargraph(float): " << fLabel << " [min=" << fMin << ":max=" << fMax << ":cur=" << *fZone << "]";
            strcpy(buffer, s.str().c_str());
        }
        
        virtual FAUSTFLOAT getValue() 
        { 
            if (*fZone != fCurrent) {
                fCurrent = *fZone;
                return fCurrent;
            } else {
                return NAN; 
            }
        }
};

/*--------------------------------------------------------------------------*/

#define MULTI_SIZE  256

class mspUI : public UI
{
	private:

        map<string, mspUIObject*> fUITable1;       // Table using labels
        map<string, mspUIObject*> fUITable2;       // Table using complete path
        map<string, mspUIObject*> fUITable3;       // Table containing bargraph
       
        map<const char*, const char*> fDeclareTable;
        std::vector<std::string> fControlsLevel;
        
        FAUSTFLOAT* fMultiTable[MULTI_SIZE];
        int fMultiIndex;
        int fMultiControl;
        
        std::string buildPath(const std::string& label) 
        {
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res += fControlsLevel[i];
                res += "/";
            }
            res += label;
            replace(res.begin(), res.end(), ' ', '_');
            return res;
        }
    
        string createLabel(const char* label)
        {
            map<const char*, const char*>::reverse_iterator it;
            if (fDeclareTable.size() > 0) {
                unsigned int i = 0;
                string res = string(label);
                char sep = '[';
                for (it = fDeclareTable.rbegin(); it != fDeclareTable.rend(); it++, i++) {
                    res = res + sep + (*it).first + ":" + (*it).second;
                    sep = ',';
                }
                res += ']';
                fDeclareTable.clear();
                return res;
            } else {
                return string(label);
            }
        }

	public:
    
		typedef map<string,mspUIObject*>::iterator iterator;

		mspUI() 
        {
     		for (int i = 0; i < MULTI_SIZE; i++) {
                fMultiTable[i] = 0;
            }
            fMultiIndex = fMultiControl = 0;
        }
		virtual ~mspUI()
		{
            clear();
   		}
      
		void addButton(const char* label, FAUSTFLOAT* zone) 
        {
            mspUIObject* obj = new mspButton(createLabel(label), zone);
            fUITable1[string(label)] = obj;
            fUITable2[buildPath(label)] = obj;
        }
        
        void addCheckButton(const char* label, FAUSTFLOAT* zone) 
        {
            mspUIObject* obj = new mspCheckButton(createLabel(label), zone);
            fUITable1[string(label)] = obj;
            fUITable2[buildPath(label)] = obj; 
        }
        
        void addSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            mspUIObject* obj = new mspSlider(createLabel(label), zone, init, min, max, step);
            fUITable1[string(label)] = obj;
            fUITable2[buildPath(label)] = obj; 
        }
        
        void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addSlider(label, zone, init, min, max, step);
        }
        
        void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addSlider(label, zone, init, min, max, step);
        }
        
        void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            mspUIObject* obj = new mspSlider(createLabel(label), zone, init, min, max, step);
            fUITable1[string(label)] = obj;
            fUITable2[buildPath(label)] = obj;
        }
        
        void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 
        {   
            fUITable3[buildPath(label)] = new mspBargraph(createLabel(label), zone, min, max);
            fDeclareTable.clear();
        }
        void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 
        {
            fUITable3[buildPath(label)] = new mspBargraph(createLabel(label), zone, min, max);
            fDeclareTable.clear();
        }
    
        void openTabBox(const char* label) {fControlsLevel.push_back(label); fDeclareTable.clear();}
        void openHorizontalBox(const char* label) {fControlsLevel.push_back(label); fDeclareTable.clear();}
        void openVerticalBox(const char* label) {fControlsLevel.push_back(label); fDeclareTable.clear();}
        void closeBox() {fControlsLevel.pop_back(); fDeclareTable.clear();}

        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {
            if (strcmp(key,"multi") == 0) {
                int index = atoi(val);
                if (index >= 0 && index < MULTI_SIZE) {
                    fMultiTable[index] = zone;
                    fMultiControl++;
                } else {
                    post("Invalid multi index = %d", index);
                }
            }
            
            fDeclareTable[key] = val;
        }
        
        void setMultiValues(FAUSTFLOAT* multi, int buffer_size)
		{
            for (int read = 0; read < buffer_size; read++) {
                int write = (fMultiIndex + read) & (MULTI_SIZE - 1);
                if (fMultiTable[write]) {
                    *fMultiTable[write] = multi[read];
                }
            }
            fMultiIndex += buffer_size;
		}
        
        bool isMulti() { return fMultiControl > 0; }
    
        bool isValue(string name) 
        {
            return (fUITable1.count(name) || fUITable2.count(name));
        }
        bool isOutputValue(string name) 
        {
            return fUITable3.count(name);
        }
        bool isInputValue(string name) 
        {
            return fUITable2.count(name);
        }
        bool setValue(string name, FAUSTFLOAT f)
        {
            if (fUITable1.count(name)) {
                fUITable1[name]->setValue(f);
                return true;
            } else if (fUITable2.count(name)) {
                fUITable2[name]->setValue(f);
                return true;
            } else {
                return false;
            }
        }
		FAUSTFLOAT getOutputValue(string name) { return fUITable3[name]->getValue(); }
          
        iterator begin1()	{ return fUITable1.begin(); }
        iterator end1()		{ return fUITable1.end(); }
        
        iterator begin2()	{ return fUITable2.begin(); }
        iterator end2()		{ return fUITable2.end(); }

        int itemsCount() { return fUITable1.size(); }
        void clear() 
        { 
            iterator it;
            for (it = begin1(); it != end1(); it++) {
                delete (*it).second;
            }
            fUITable1.clear(); 
            fUITable2.clear(); 
        }
};

//--------------------------------------------------------------------------
static bool check_digit(const string& name)
{
    for (int i = name.size() - 1; i >= 0; i--) {
        if (isdigit(name[i])) return true;
    }
    return false;
}

static int count_digit(const string& name)
{
    int count = 0;
    for (int i = name.size() - 1; i >= 0; i--) {
        if (isdigit(name[i])) count++;
    }
    return count;
}

void faust_anything(t_faust* obj, t_symbol* s, short ac, t_atom* av)
{
    bool res = false;
    string name = string((s)->s_name);
     
    if (ac < 0) return;
    
    // Check if no argument is there, consider it is a toggle message for a button
    if (ac == 0 && obj->dspUI->isValue(name)) {
        
        string name = string((s)->s_name);
        float off = 0.0f;
        float on = 1.0f;
        obj->dspUI->setValue(name, off);
        obj->dspUI->setValue(name, on);
        
        av[0].a_type = A_FLOAT;
        av[0].a_w.w_float = off;
        faust_anything(obj, s, 1, av);
        
        return;
    }
    
    // List of values
    if (check_digit(name)) {
        
        int ndigit = 0;
        int pos;
        
        for (pos = name.size() - 1; pos >= 0; pos--) {
            if (isdigit(name[pos]) || name[pos] == ' ') {
                ndigit++;
            } else {
                break;
            }
        }
        pos++;
        
        string prefix = name.substr(0, pos);
        string num_base = name.substr(pos);
        int num = atoi(num_base.c_str());
        
        int i;
        t_atom* ap;
       
        // Increment ap each time to get to the next atom
        for (i = 0, ap = av; i < ac; i++, ap++) {
            float value;
            switch (atom_gettype(ap)) {
                case A_LONG: {
                    value = (float)ap[0].a_w.w_long;
                    break;
                }
                case A_FLOAT:
                    value = ap[0].a_w.w_float;
                    break;
                    
                default:
                    post("Invalid argument in parameter setting"); 
                    return;         
            }
            
            stringstream num_val;
            num_val << num + i;
            char param_name[256];
            
            switch (ndigit - count_digit(num_val.str())) {
                case 0: 
                    sprintf(param_name, "%s%s", prefix.c_str(), num_val.str().c_str());
                    break;
                case 1: 
                    sprintf(param_name, "%s %s", prefix.c_str(), num_val.str().c_str());
                    break;
                case 2: 
                    sprintf(param_name, "%s  %s", prefix.c_str(), num_val.str().c_str());
                    break;
            }
            
            // Try special naming scheme for list of parameters
            res = obj->dspUI->setValue(param_name, value); 
            
            // Otherwise try standard name
            if (!res) {
                res = obj->dspUI->setValue(name, value);
            }
            
            if (!res) {
                post("Unknown parameter : %s", (s)->s_name);
            }
        }
    } else {
        // Standard parameter name
        float value = (av[0].a_type == A_LONG) ? (float)av[0].a_w.w_long : av[0].a_w.w_float;
        res = obj->dspUI->setValue(name, value);
    }
    
    if (!res) {
        post("Unknown parameter : %s", (s)->s_name);
    }
}

/*--------------------------------------------------------------------------*/
void faust_create_jsui(t_faust* x)
{
    t_object *patcher, *box, *obj;
    object_obex_lookup((t_object*)x, gensym("#P"), &patcher);
    
    for (box = jpatcher_get_firstobject(patcher); box; box = jbox_get_nextobject(box)) {
        obj = jbox_get_object(box);
        // Notify JSON
        if (obj && strcmp(object_classname(obj)->s_name, "js") == 0) {
            t_atom json;
            atom_setsym(&json, gensym(x->m_json));
            object_method_typed(obj, gensym("anything"), 1, &json, 0);
            post("Generate UI from JSON...");
        }
    }
        
    // Keep all outputs
    x->m_output_table.clear();
    for (box = jpatcher_get_firstobject(patcher); box; box = jbox_get_nextobject(box)) {
        obj = jbox_get_object(box);
        t_symbol *scriptingname = jbox_get_varname(obj); // scripting name
        if (scriptingname && x->dspUI->isOutputValue(scriptingname->s_name)) {
            x->m_output_table[scriptingname->s_name] = obj;
        }
    }
}

void faust_update_outputs(t_faust* x)
{
    map<string, t_object*>::iterator it;
    for (it =  x->m_output_table.begin(); it != x->m_output_table.end(); it++) {
        FAUSTFLOAT value = x->dspUI->isOutputValue((*it).first);
        if (value != NAN) {
            t_atom at_value;
            atom_setfloat(&at_value, value);
            object_method_typed((*it).second, gensym("float"), 1, &at_value, 0);
        }
    }
}

/*--------------------------------------------------------------------------*/
void* faust_new(t_symbol* s, short ac, t_atom* av)
{
    t_faust* x = (t_faust*)newobject(faust_class);

    x->m_mute = false;

    x->dsp = new mydsp();
    x->dspUI = new mspUI();

    x->dsp->init(long(sys_getsr()));
    x->dsp->buildUserInterface(x->dspUI);
    
    // Prepare JSON
    JSONUI builder(x->dsp->getNumInputs(), x->dsp->getNumOutputs());
    x->dsp->metadata(&builder);
    x->dsp->buildUserInterface(&builder);
    x->m_json = strdup(builder.JSON().c_str());
    
    int num_input;
    
    if (x->dspUI->isMulti()) {
        num_input = x->dsp->getNumInputs() + 1;
    } else {
        num_input = x->dsp->getNumInputs();
    }
    
    x->args = (void**)calloc((num_input + x->dsp->getNumOutputs()) + 2, sizeof(void*));
    /* Multi in */
    dsp_setup((t_pxobject*)x, num_input);

    /* Multi out */
    for (int i = 0; i< x->dsp->getNumOutputs(); i++) {
        outlet_new((t_pxobject*)x, (char*)"signal");
    }

    ((t_pxobject*)x)->z_misc = Z_NO_INPLACE; // To assure input and output buffers are actually different
    
    // send JSON to JS script
    faust_create_jsui(x);
    return x;
}

/*--------------------------------------------------------------------------*/
void faust_assist(t_faust* x, void* b, long msg, long a, char* dst)
{
    if (msg == ASSIST_INLET) {
        if (a == 0) {
            if (x->dsp->getNumInputs() == 0) {
                sprintf(dst, "(signal) : Unused Input");
            } else {
                sprintf(dst, "(signal) : Audio Input %ld", (a+1));
			}
			post((char*)"------------------");
			for (mspUI::iterator it = x->dspUI->begin1(); it != x->dspUI->end1(); ++it) {
				char param[256];
				it->second->toString(param);
				post(param);
			}
        } else if (a < x->dsp->getNumInputs()) {
            sprintf(dst, "(signal) : Audio Input %ld", (a+1));
        }
    } else if (msg == ASSIST_OUTLET) {
        sprintf(dst, "(signal) : Audio Output %ld", (a+1));
    }
}

/*--------------------------------------------------------------------------*/
void faust_mute(t_faust* obj, t_symbol* s, short ac, t_atom* at)
{
    if (atom_gettype(at) == A_LONG) {
        obj->m_mute = atom_getlong(at);
    }
}

/*--------------------------------------------------------------------------*/
void faust_free(t_faust* x)
{
	dsp_free((t_pxobject*)x);
	if (x->dsp) delete x->dsp;
	if (x->dspUI) delete x->dspUI;
	if (x->args) free(x->args);
    if (x->m_json) free(x->m_json);
}

/*--------------------------------------------------------------------------*/
void faust_perform64(t_faust* x, t_object* dsp64, double** ins, long numins, double** outs, long numouts, long sampleframes, long flags, void* userparam)
{
    AVOIDDENORMALS;
    if (x->m_mute) {
        // Write null buffers to outs
        for (int i = 0; i < numouts; i++) {
             memset(outs[i], 0, sizeof(double) * sampleframes);
        }
    } else {
        if (x->dspUI->isMulti()) {
            x->dspUI->setMultiValues(ins[0], sampleframes);
            x->dsp->compute(sampleframes, ++ins, outs);
        } else {
            x->dsp->compute(sampleframes, ins, outs);
        }
        faust_update_outputs(x);
    }
}

/*--------------------------------------------------------------------------*/
void faust_dsp64(t_faust* x, t_object* dsp64, short* count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, faust_perform64, 0, NULL);
}

/*--------------------------------------------------------------------------*/
extern "C" int main(void)
{
	setup((t_messlist**)&faust_class, (method)faust_new, (method)faust_free,
		(short)sizeof(t_faust), 0L, A_DEFFLOAT, 0);

	dsp* thedsp = new mydsp();
    mspUI dspUI;
	thedsp->buildUserInterface(&dspUI);

    // 03/11/14 : use 'anything' to handle all parameter changes
    addmess((method)faust_anything, (char*)"anything", A_GIMME, 0);

    addmess((method)faust_dsp64, (char*)"dsp64", A_CANT, 0);
	addmess((method)faust_assist, (char*)"assist", A_CANT, 0);
    addmess((method)faust_mute, (char*)"mute", A_GIMME, 0);
	dsp_initclass();

	post((char*)"Faust DSP object 64 bits v%s", EXTERNAL_VERSION);
    post((char*)"Copyright (c) 2012-2014 Grame");
    Max_Meta1 meta1;
    mydsp::metadata(&meta1);
    if (meta1.fCount > 0) {
        Max_Meta2 meta2;
        post("------------------------------");
        mydsp::metadata(&meta2);
        post("------------------------------");
    }
    
    delete(thedsp);
	return 0;
}

/********************END ARCHITECTURE SECTION (part 2/2)****************/




