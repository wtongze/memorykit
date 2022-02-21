{
  "targets": [
    {
      "target_name": "test",
      'type': 'executable',
      "sources": ["src/test.cpp"],
    },
    {
      "target_name": "memorykit-native",
      "sources": [
        "src/memorykit.cpp",
        "src/process/common.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags_cc": [
        "-std=c++2a",
        "-fexceptions",
      ],
      "defines": [
        "NAPI_CPP_EXCEPTIONS"
      ],
      "xcode_settings": {
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7",
        "CLANG_CXX_LANGUAGE_STANDARD": "c++20",
        "OTHER_CPLUSPLUSFLAGS": ["-fexceptions"]
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
              "src/common/linux.cpp",
              "src/process/linux.cpp"
            ]
          }
        ],
        [
          "OS==\"mac\"",
          {
            "sources": [
              "src/common/darwin.cpp"
            ]
          }
        ],
        [
          "OS==\"win\"",
          {
            "sources": [
              "src/common/windows.cpp"
            ]
          }
        ]
      ]
    }
  ]
}
