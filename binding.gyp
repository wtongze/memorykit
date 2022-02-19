{
  "targets": [
    {
      "target_name": "memorykit-native",
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags": [
        "-std=c17",
        "-fexceptions"
      ],
      "cflags_cc": [
        "-std=c++20",
        "-fexceptions"
      ],
      "defines": [
        "NAPI_CPP_EXCEPTIONS"
      ],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7"
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1
        }
      },
      "conditions": [
        [
          "OS==\"linux\"",
          {
            "sources": [
              "src/memorykit.cpp",
              "src/common/linux.cpp"
            ]
          }
        ]
      ]
    }
  ]
}
