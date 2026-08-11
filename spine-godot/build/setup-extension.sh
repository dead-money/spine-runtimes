#!/bin/bash
set -e

dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
pushd "$dir" > /dev/null

if [ $# -lt 2 ] || [ $# -gt 3 ]; then
    echo "Usage: ./setup-extension.sh <Godot version> <dev:true|false>"
    echo
    echo "e.g.:"
    echo "       ./setup-extension.sh 4.2.2-stable true"

    exit 1
fi

godot_branch=${1%/}
dev=${2%/}
mono=false
godot_cpp_repo=https://github.com/godotengine/godot-cpp.git
godot_repo=https://github.com/godotengine/godot.git
godot_cpp_revision=${GODOT_CPP_REVISION:-}

if [[ $# -eq 3 ]]; then
    mono=${3%/}
fi

if [ "$dev" != "true" ] && [ "$dev" != "false" ]; then
    echo "Invalid value for the 'dev' argument. It should be either 'true' or 'false'."
    exit 1
fi

if [ "$mono" != "true" ] && [ "$mono" != "false" ]; then
    echo "Invalid value for the 'mono' argument. It should be either 'true' or 'false'."
    exit 1
fi

godot_cpp_branch=$(echo $godot_branch | cut -d. -f1-2 | cut -d- -f1)

if [ -z "$godot_cpp_revision" ]; then
    if ! git ls-remote --exit-code --heads $godot_cpp_repo $godot_cpp_branch > /dev/null 2>&1; then
        echo "godot-cpp branch '$godot_cpp_branch' not found, falling back to 'master'"
        godot_cpp_branch="master"
    fi
    godot_cpp_revision=$godot_cpp_branch
fi

cpus=2
if [ "$OSTYPE" == "msys" ]; then
	cpus=$NUMBER_OF_PROCESSORS
elif [[ "$OSTYPE" == "darwin"* ]]; then
	cpus=$(sysctl -n hw.logicalcpu)
else
	cpus=$(grep -c ^processor /proc/cpuinfo)
fi

echo "godot-cpp revision: $godot_cpp_revision"
echo "godot branch: $godot_branch"
echo "dev: $dev"
echo "mono: $mono"
echo "cpus: $cpus"

pushd ..

current_godot_cpp_revision=""
if [ -d godot-cpp/.git ]; then
    if revision=$(git -C godot-cpp rev-parse HEAD 2>/dev/null) &&
            git -C godot-cpp diff --quiet &&
            git -C godot-cpp diff --cached --quiet; then
        current_godot_cpp_revision=$revision
    fi
fi

if [ "$current_godot_cpp_revision" != "$godot_cpp_revision" ]; then
    rm -rf godot-cpp bin
    find . \( -name '*.o' -o -name '*.os' -o -name '*.a' \) -delete
    rm -f .sconsign.dblite

    git init godot-cpp
    git -C godot-cpp remote add origin $godot_cpp_repo
    git -C godot-cpp fetch --depth 1 origin $godot_cpp_revision
    git -C godot-cpp checkout --detach FETCH_HEAD
fi

rm -rf example-v4-extension/bin
mkdir -p example-v4-extension/bin

if [ $dev == "true" ]; then
    echo "Dev build, creating godot-cpp/dev"
    touch godot-cpp/dev
    rm -rf godot
    git clone --depth 1 $godot_repo -b $godot_branch
    pushd godot
    scons target=editor dev_build=true optimize=debug --jobs=$cpus
    popd
fi

cp spine_godot_extension.gdextension example-v4-extension/bin
rm -rf spine_godot/spine-cpp
cp -r ../spine-cpp spine_godot

popd
popd > /dev/null
