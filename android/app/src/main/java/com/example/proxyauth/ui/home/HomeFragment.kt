package com.example.proxyauth.ui.home

import android.bluetooth.BluetoothDevice
import android.content.Context
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import android.widget.ListView
import android.widget.TextView
import androidx.fragment.app.Fragment
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProvider
import com.example.proxyauth.R
import com.example.proxyauth.databinding.FragmentHomeBinding

class HomeFragment : Fragment() {

    private lateinit var homeViewModel: HomeViewModel
    private var _binding: FragmentHomeBinding? = null

    var devAdapter : ArrayAdapter<String>? = null
    private val deviceList : ArrayList<String> = ArrayList<String>()
    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        homeViewModel =
            ViewModelProvider(this).get(HomeViewModel::class.java)

        _binding = FragmentHomeBinding.inflate(inflater, container, false)
        val root: View = binding.root


        var devListView : ListView = binding.deviceList
        devAdapter = activity?.let {
            ArrayAdapter<String>(
                it,
                android.R.layout.simple_list_item_1,
                deviceList
            )
        }
        devListView.adapter = devAdapter
        updateDevList();

        return root
    }

    fun updateDevList() {
        deviceList.clear()
        deviceList.addAll(homeViewModel.fetchDeviceList())
        devAdapter?.notifyDataSetChanged()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}