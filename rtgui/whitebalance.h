/*
 *  This file is part of RawTherapee.
 *
 *  Copyright (c) 2004-2010 Gabor Horvath <hgabor@rawtherapee.com>
 *
 *  RawTherapee is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RawTherapee is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RawTherapee.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <gtkmm.h>

#include "adjuster.h"
#include "checkbox.h"
#include "guiutils.h"
#include "toolpanel.h"
#include "wbprovider.h"

#include "../rtengine/procparams.h"
#include "../rtengine/utils.h"

class SpotWBListener
{
public:
    virtual ~SpotWBListener() = default;
    virtual void spotWBRequested(int size) = 0;
};

class WhiteBalance final : public ToolParamBlock, public AdjusterListener, public CheckBoxListener, public FoldableToolPanel, public rtengine::AutoWBListener
{

    enum WB_LabelType {
        WBLT_GUI,
        WBLT_PP
    };

private:
    Gtk::Label*  StudLabel;
    Gtk::Label*  mulLabel;

protected:
    class MethodColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > colIcon;
        Gtk::TreeModelColumn<Glib::ustring> colLabel;
        Gtk::TreeModelColumn<int> colId;
        MethodColumns()
        {
            add(colIcon);
            add(colLabel);
            add(colId);
        }
    };
    
    rtengine::ProcEvent EvWBObserver10;
    rtengine::ProcEvent EvWBitcwbthres;
    rtengine::ProcEvent EvWBitcwbnopurple;
    rtengine::ProcEvent EvWBitcwbsorted;
    rtengine::ProcEvent EvWBitcwbforceextra;
    rtengine::ProcEvent EvWBitcwbprim;
    rtengine::ProcEvent EvWBitcwbprecis;
    rtengine::ProcEvent EvWBitcwbsize;
    rtengine::ProcEvent EvWBitcwbdelta;
    rtengine::ProcEvent EvWBitcwbfgreen;
    rtengine::ProcEvent EvWBitcwbrgreen;
    rtengine::ProcEvent EvWBitcwbsampling;
    rtengine::ProcEvent EvWBitcwbobs;

    static Glib::RefPtr<Gdk::Pixbuf> wbPixbufs[rtengine::toUnderlying(rtengine::procparams::WBEntry::Type::CUSTOM) + 1];
    Glib::RefPtr<Gtk::TreeStore> refTreeModel;
    MethodColumns methodColumns;
    MyComboBox* method;
    Gtk::Button* resetButton;
    MyComboBoxText* spotsize;
    Adjuster* temp;
    Adjuster* green;
    Adjuster* equal;
    Adjuster* tempBias;
    CheckBox* observer10;
    Gtk::Frame* itcwbFrame;
    Adjuster* itcwb_thres;
    Adjuster* itcwb_precis;
    Adjuster* itcwb_size;
    Adjuster* itcwb_delta;
    Adjuster* itcwb_fgreen;
    Adjuster* itcwb_rgreen;
    
    Gtk::CheckButton* itcwb_nopurple;
    Gtk::CheckButton* itcwb_obs;
    Gtk::CheckButton* itcwb_sorted;
    Gtk::CheckButton* itcwb_forceextra;
    MyComboBoxText* itcwb_prim;
    
    Gtk::CheckButton* itcwb_sampling;
    bool lastitcwb_nopurple;
    bool lastitcwb_obs;
    bool lastitcwb_sorted;
    bool lastitcwb_forceextra;
    bool lastitcwb_sampling;

    Gtk::Button* spotbutton;
    int opt;
    double nextTemp;
    double nextGreen;
    WBProvider *wbp;  // pointer to a ToolPanelCoordinator object, or its subclass BatchToolPanelCoordinator
    SpotWBListener* wblistener;
    sigc::connection methconn, itcwb_nopurpleconn, itcwb_obsconn, itcwb_sortedconn,  itcwb_forceextraconn, itcwb_samplingconn, itcwb_primconn;
    int custom_temp;
    double custom_green;
    double custom_equal;

    IdleRegister idle_register;

    void cache_customWB    (int temp, double green); //cache custom WB setting to allow its recall
    void cache_customTemp  (int temp);               //cache Temperature only to allow its recall
    void cache_customGreen (double green);           //cache Green only to allow its recall
    void cache_customEqual (double equal);           //cache Equal only to allow its recall

    int  setActiveMethod   (Glib::ustring label);
    int _setActiveMethod   (Glib::ustring &label, Gtk::TreeModel::Children &children);

    Gtk::TreeModel::Row                                   getActiveMethod();
    unsigned int                                          findWBEntryId  (const Glib::ustring& label, enum WB_LabelType lblType = WBLT_GUI);
    std::pair<bool, const rtengine::procparams::WBEntry&> findWBEntry    (const Glib::ustring& label, enum WB_LabelType lblType = WBLT_GUI);

public:
    static const Glib::ustring TOOL_NAME;

    WhiteBalance ();
    ~WhiteBalance () override;

    static void init    ();
    static void cleanup ();
    void read           (const rtengine::procparams::ProcParams* pp, const ParamsEdited* pedited = nullptr) override;
    void write          (rtengine::procparams::ProcParams* pp, ParamsEdited* pedited = nullptr) override;
    void setDefaults    (const rtengine::procparams::ProcParams* defParams, const ParamsEdited* pedited = nullptr) override;
    void setBatchMode   (bool batchMode) override;

    void optChanged ();
    void spotPressed ();
    void spotSizeChanged ();
    void adjusterChanged(Adjuster* a, double newval) override;
    void checkBoxToggled(CheckBox* c, CheckValue newval) override;
    int  getSize ();
    void setWBProvider (WBProvider* p)
    {
        wbp = p;
    }
    void setSpotWBListener (SpotWBListener* l)
    {
        wblistener = l;
    }
    void setWB (int temp, double green);
    void resetWB ();
    void WBChanged           (double temp, double green, double rw, double gw, double bw, float studgood) override;
    void itcwb_nopurple_toggled ();
    void itcwb_obs_toggled ();
    void itcwb_sorted_toggled ();
    void itcwb_forceextra_toggled ();
    void itcwb_prim_changed ();
    void itcwb_sampling_toggled ();

    void setAdjusterBehavior (bool tempadd, bool greenadd, bool equaladd, bool tempbiasadd);
    void trimValues          (rtengine::procparams::ProcParams* pp) override;
    void enabledChanged() override;
};
