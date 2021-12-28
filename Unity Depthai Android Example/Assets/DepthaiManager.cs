using System;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.UI;

public class DepthaiManager : MonoBehaviour
{
    [SerializeField] private Text RGBFrameNumText;
    [SerializeField] private RawImage RgbImage;
    [SerializeField] private Text DisparityFrameNumText;
    [SerializeField] private RawImage ColorDisparityImage;
    
    private const int RGBWidth = 640;
    private const int RGBHeight = 480;
    private const int DisparityWidth = 640;
    private const int DisparityHeight = 400;
    
    private Texture2D _rgbImgTexture;
    private Texture2D _disparityImgTexture;
    
    private Color32[] _rgbImgData;
    private Color32[] _disparityImgData;
    
    private GCHandle _rgbImgHandle;
    private GCHandle _disparityImgHandle;
    
    private IntPtr _rgbImgPtr;
    private IntPtr _disparityImgPtr;
    
    private bool _deviceRunning = false;
    
    // Start is called before the first frame update
    private void Start()
    {
        InitTextures();
        ConnectDevice();
        
        // Pass the textures to the Raw images
        RgbImage.texture = _rgbImgTexture;
        ColorDisparityImage.texture = _disparityImgTexture;
    }

    // Update is called once per frame
    private void Update()
    {
        if(!_deviceRunning) return;
        
        var rgbFrameNum = api_get_rgb_image(_rgbImgPtr);
        var disparityFrameNum = api_get_color_disparity_image(_disparityImgPtr);

        // Update frame number text
        RGBFrameNumText.text = $"RGB Frame: {rgbFrameNum}";
        DisparityFrameNumText.text = $"Disparity Frame: {disparityFrameNum}";
        
        // Update images
        _rgbImgTexture.SetPixels32(_rgbImgData);
        _disparityImgTexture.SetPixels32(_disparityImgData);
        _rgbImgTexture.Apply();
        _disparityImgTexture.Apply();
    }
    
    private void InitTextures()
    {
        // Initialize the output textures
        _rgbImgTexture = new Texture2D(RGBWidth, RGBHeight, TextureFormat.ARGB32, false);
        _disparityImgTexture = new Texture2D(DisparityWidth, DisparityHeight, TextureFormat.ARGB32, false);
        
        // Initialize the output image container arrays
        _rgbImgData = _rgbImgTexture.GetPixels32();
        _disparityImgData = _disparityImgTexture.GetPixels32();
        
        // Pin pixel32 array
        _rgbImgHandle = GCHandle.Alloc(_rgbImgData, GCHandleType.Pinned);
        _disparityImgHandle = GCHandle.Alloc(_disparityImgData, GCHandleType.Pinned);
        
        // Get the pinned address
        _rgbImgPtr = _rgbImgHandle.AddrOfPinnedObject();
        _disparityImgPtr = _disparityImgHandle.AddrOfPinnedObject();
    }
    
    public void ConnectDevice()
    {
        // Initialize native API
        api_start_device(RGBWidth, RGBHeight);
        _deviceRunning = true;
    }
    
    
    [DllImport("depthai_android_api")]
    private static extern void api_start_device(int rgbWidth, int rgbHeight);
    
    [DllImport("depthai_android_api")]
    private static extern uint api_get_rgb_image(IntPtr rgbImagePtr);
    
    [DllImport("depthai_android_api")]
    private static extern uint api_get_color_disparity_image(IntPtr disparityImagePtr);
}
