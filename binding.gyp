{
  "targets": [
    {
      "target_name": "md-camera-recognition",
      "sources": [
        "src/common/camera_common.cc",
        "src/common/platform_factory.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "conditions": [
        [
          "OS=='win'",
          {
            "sources": [
              "src/windows/camera_windows.cc"
            ],
            "libraries": [
              "-lstrmiids",
              "-lole32",
              "-loleaut32",
              "setupapi.lib",
              "propsys.lib"
            ],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "ExceptionHandling": 1,
                "AdditionalOptions": [ "/utf-8" ]
              }
            }
          }
        ],
        [
          "OS=='mac'",
          {
            "sources": [
              "src/macos/camera_type2.mm"
            ],
            "link_settings": {
              "libraries": [
                "-framework AVFoundation",
                "-framework CoreMedia",
                "-framework CoreMediaIO",
                "-framework Foundation",
                "-framework IOKit"
              ]
            },
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              "CLANG_CXX_LIBRARY": "libc++",
              "MACOSX_DEPLOYMENT_TARGET": "10.15",
              "CLANG_ENABLE_OBJC_ARC": "YES"
            }
          }
        ]
      ]
    }
  ]
}
