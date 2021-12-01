package com.miya.player

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import android.widget.Toast
import com.miya.player.databinding.ActivityMainBinding
import java.io.File

class MainActivity : AppCompatActivity() {

    private val mBinding by lazy { ActivityMainBinding.inflate(layoutInflater) }
    private val mMiyaPlayer by lazy { MiyaPlayer() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(mBinding.root)

        mMiyaPlayer.setSurfaceView(mBinding.surfaceView)
        mMiyaPlayer.dataSource = File("$filesDir${File.separator}video_1.mp4").absolutePath
        mMiyaPlayer.setOnPreparedListener(object : OnPreparedListener {
            override fun onPrepared() {
                runOnUiThread {
                    Toast.makeText(this@MainActivity, "准备完成，即将开始播放", Toast.LENGTH_SHORT).show()
                }
                mMiyaPlayer.start()
            }
        })

        mMiyaPlayer.setOnErrorListener(object : OnErrorListener {
            override fun onError(errorMsg: String) {
                runOnUiThread {
                    Toast.makeText(this@MainActivity, errorMsg, Toast.LENGTH_SHORT).show()
                }
            }
        })
    }

    override fun onResume() {
        super.onResume()
        mMiyaPlayer.prepare()
    }

    override fun onStop() {
        super.onStop()
        mMiyaPlayer.stop()
    }

    override fun onDestroy() {
        super.onDestroy()
        mMiyaPlayer.release()
    }
}