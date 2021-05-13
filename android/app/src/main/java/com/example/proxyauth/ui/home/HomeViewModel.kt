package com.example.proxyauth.ui.home

import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class HomeViewModel : ViewModel() {

    private val _text = MutableLiveData<String>().apply {
        value = "This is home Fragment"
    }
    private val trustedDevices : ArrayList<String> = ArrayList ()
    /*
    * Fetch all the devices that are trusted and connected
    */
    fun fetchDeviceList() : ArrayList<String> {
        trustedDevices.add("device 1")
        trustedDevices.add("device 2")
        Log.i("test", trustedDevices.toString())
        return trustedDevices
    }
}