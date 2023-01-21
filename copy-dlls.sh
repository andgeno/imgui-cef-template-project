#!/usr/bin/env bash

#CEF_VERSION="106.0.26+ge105400+chromium-106.0.5249.91"
CEF_VERSION="109.1.16+g454cbc2+chromium-109.0.5414.87"

CEF_PLATFORM="windows64"
CEF_DIR="vendor/cef/third_party/cef/cef_binary_${CEF_VERSION}_${CEF_PLATFORM}"

for type in Debug Release
do
    echo -e "\033[0;32;1m*\033[0m Copying DLLs for \033[0;33;1m${type}\033[0m ..."
    (
        TYPE_DIR="cmake-build-${type,,}"
        if [[ -d "$TYPE_DIR" ]]
        then
            cd "$TYPE_DIR" || exit

            (
              cp -v "$CEF_DIR/$type"/* "src/app/"
              cp -rv "$CEF_DIR/Resources"/* "src/app/"
            ) | while read -r LINE
            do
                echo -e "  \033[0;35;1m*\033[0m $LINE"
            done
        else
            echo "Skipped."
        fi
    )
    echo
done

