#!/usr/bin/env bash
set -euo pipefail

APP_SLUG="desktop-serial"
APP_NAME="Desktop Serial Free"
BIN_NAME="desktop_serial"

BASE_DIR="/opt"
CURRENT_LINK="${BASE_DIR}/${APP_SLUG}"
BIN_LINK="/usr/local/bin/${APP_SLUG}"
DESKTOP_FILE="/usr/share/applications/${APP_SLUG}.desktop"
ICON_BASENAME="${APP_SLUG}"
ICON_DIR_HICOLOR="/usr/share/icons/hicolor"

SUDO=""
TMP_DIR=""

log() {
    printf '[*] %s\n' "$*"
}

warn() {
    printf '[!] %s\n' "$*" >&2
}

die() {
    printf '[x] %s\n' "$*" >&2
    exit 1
}

cleanup() {
    if [[ -n "${TMP_DIR}" && -d "${TMP_DIR}" ]]; then
        rm -rf "${TMP_DIR}"
    fi
}
trap cleanup EXIT

need_cmd() {
    command -v "$1" >/dev/null 2>&1 || die "Thiếu lệnh: $1"
}

run_root() {
    ${SUDO} "$@"
}

init_sudo() {
    if [[ "${EUID}" -ne 0 ]]; then
        SUDO="sudo"
        need_cmd sudo
    fi
}

usage() {
    cat <<'EOF'
Cách dùng:
  ./desktop-serial-manager.sh install /duong-dan/project.zip [version]
  ./desktop-serial-manager.sh upgrade /duong-dan/project.zip [version]
  ./desktop-serial-manager.sh uninstall
  ./desktop-serial-manager.sh status

Ví dụ:
  ./desktop-serial-manager.sh install /mnt/data/desktop-serial\(1\).zip 1.0.0
  ./desktop-serial-manager.sh upgrade ./desktop-serial.zip 1.1.0
  ./desktop-serial-manager.sh uninstall
  ./desktop-serial-manager.sh status
EOF
}

install_build_dependencies() {
    log "Cài dependency build"
    run_root apt update
    run_root apt install -y \
        build-essential \
        cmake \
        ninja-build \
        unzip \
        desktop-file-utils \
        qt6-base-dev \
        qt6-base-dev-tools \
        qt6-serialport-dev
}

refresh_desktop_caches() {
    if command -v update-desktop-database >/dev/null 2>&1; then
        run_root update-desktop-database /usr/share/applications || true
    fi

    if command -v gtk-update-icon-cache >/dev/null 2>&1; then
        run_root gtk-update-icon-cache -f /usr/share/icons/hicolor || true
    fi
}

extract_project() {
    local zip_path="$1"

    [[ -f "${zip_path}" ]] || die "Không tìm thấy file zip: ${zip_path}"

    TMP_DIR="$(mktemp -d)"
    unzip -q "${zip_path}" -d "${TMP_DIR}"

    local project_dir
    project_dir="$(find "${TMP_DIR}" -mindepth 1 -maxdepth 1 -type d | head -n 1)"

    [[ -n "${project_dir}" ]] || die "Không tìm thấy thư mục project sau khi giải nén"
    [[ -f "${project_dir}/CMakeLists.txt" ]] || die "Không thấy CMakeLists.txt trong project"

    printf '%s\n' "${project_dir}"
}

detect_version() {
    local project_dir="$1"
    local explicit_version="${2:-}"

    if [[ -n "${explicit_version}" ]]; then
        printf '%s\n' "${explicit_version}"
        return
    fi

    local version
    version="$(sed -n 's/.*project([^)]*VERSION[[:space:]]\+\([0-9][0-9.]*\).*/\1/p' "${project_dir}/CMakeLists.txt" | head -n 1)"

    [[ -n "${version}" ]] || die "Không tự dò được version. Hãy truyền version ở tham số thứ 3."

    printf '%s\n' "${version}"
}

build_project() {
    local project_dir="$1"

    log "Configure project"
    cmake -S "${project_dir}" -B "${project_dir}/build-linux" -G Ninja -DCMAKE_BUILD_TYPE=Release

    log "Build project"
    cmake --build "${project_dir}/build-linux" --config Release

    [[ -f "${project_dir}/build-linux/${BIN_NAME}" ]] || die "Không tìm thấy binary sau khi build: ${project_dir}/build-linux/${BIN_NAME}"
}

install_icon() {
    local project_dir="$1"

    local icon_source=""
    local icon_target=""

    if [[ -f "${project_dir}/assets/icon.png" ]]; then
        icon_source="${project_dir}/assets/icon.png"
        icon_target="${ICON_DIR_HICOLOR}/256x256/apps/${ICON_BASENAME}.png"
    elif [[ -f "${project_dir}/assets/icon_256.png" ]]; then
        icon_source="${project_dir}/assets/icon_256.png"
        icon_target="${ICON_DIR_HICOLOR}/256x256/apps/${ICON_BASENAME}.png"
    elif [[ -f "${project_dir}/assets/icon_128.png" ]]; then
        icon_source="${project_dir}/assets/icon_128.png"
        icon_target="${ICON_DIR_HICOLOR}/128x128/apps/${ICON_BASENAME}.png"
    else
        warn "Không tìm thấy icon trong assets/, bỏ qua"
        return
    fi

    run_root install -Dm644 "${icon_source}" "${icon_target}"
}

write_desktop_entry() {
    log "Tạo launcher .desktop"

    run_root tee "${DESKTOP_FILE}" >/dev/null <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=${APP_NAME}
Comment=Desktop serial communication tool
Exec=${CURRENT_LINK}/${BIN_NAME}
Icon=${ICON_BASENAME}
Terminal=false
Categories=Development;Utility;
StartupNotify=true
EOF

    run_root chmod 644 "${DESKTOP_FILE}"
}

install_or_upgrade() {
    local zip_path="$1"
    local explicit_version="${2:-}"

    need_cmd unzip
    need_cmd cmake
    need_cmd sed
    need_cmd find

    install_build_dependencies

    local project_dir
    project_dir="$(extract_project "${zip_path}")"

    local version
    version="$(detect_version "${project_dir}" "${explicit_version}")"

    local versioned_dir="${BASE_DIR}/${APP_SLUG}-${version}"

    log "Version sẽ cài: ${version}"
    build_project "${project_dir}"

    log "Cài files vào ${versioned_dir}"
    run_root rm -rf "${versioned_dir}"
    run_root mkdir -p "${versioned_dir}"
    run_root install -m755 "${project_dir}/build-linux/${BIN_NAME}" "${versioned_dir}/${BIN_NAME}"

    if [[ -d "${project_dir}/config" ]]; then
        run_root mkdir -p "${versioned_dir}/config"
        run_root cp -r "${project_dir}/config/." "${versioned_dir}/config/"
    fi

    if [[ -d "${project_dir}/firmware" ]]; then
        run_root mkdir -p "${versioned_dir}/firmware"
        run_root cp -r "${project_dir}/firmware/." "${versioned_dir}/firmware/"
    fi

    install_icon "${project_dir}"

    log "Cập nhật symlink hiện tại"
    run_root ln -sfn "${versioned_dir}" "${CURRENT_LINK}"
    run_root ln -sfn "${CURRENT_LINK}/${BIN_NAME}" "${BIN_LINK}"

    write_desktop_entry
    refresh_desktop_caches

    log "Hoàn tất"
    printf 'Current version: %s\n' "${version}"
    printf 'Run: %s\n' "${BIN_LINK}"
}

show_status() {
    printf 'App: %s\n' "${APP_NAME}"
    printf 'Current link: %s\n' "${CURRENT_LINK}"

    if [[ -L "${CURRENT_LINK}" ]]; then
        printf 'Current target: %s\n' "$(readlink -f "${CURRENT_LINK}")"
    else
        printf 'Current target: <not installed>\n'
    fi

    printf 'Binary link: %s\n' "${BIN_LINK}"
    if [[ -L "${BIN_LINK}" ]]; then
        printf 'Binary target: %s\n' "$(readlink -f "${BIN_LINK}")"
    else
        printf 'Binary target: <not installed>\n'
    fi

    printf 'Desktop file: %s\n' "${DESKTOP_FILE}"
    [[ -f "${DESKTOP_FILE}" ]] && printf 'Desktop entry: present\n' || printf 'Desktop entry: missing\n'

    printf 'Installed versions:\n'
    find "${BASE_DIR}" -maxdepth 1 -type d -name "${APP_SLUG}-*" 2>/dev/null | sort || true
}

uninstall_all() {
    log "Gỡ launcher, symlink, icon, toàn bộ version đã cài"

    run_root rm -f "${BIN_LINK}"
    run_root rm -f "${DESKTOP_FILE}"
    run_root rm -f "${CURRENT_LINK}"

    run_root rm -f "${ICON_DIR_HICOLOR}/256x256/apps/${ICON_BASENAME}.png"
    run_root rm -f "${ICON_DIR_HICOLOR}/128x128/apps/${ICON_BASENAME}.png"

    while IFS= read -r path; do
        [[ -n "${path}" ]] && run_root rm -rf "${path}"
    done < <(find "${BASE_DIR}" -maxdepth 1 -type d -name "${APP_SLUG}-*" 2>/dev/null | sort)

    refresh_desktop_caches
    log "Đã gỡ xong"
}

main() {
    init_sudo

    local action="${1:-}"
    case "${action}" in
        install)
            [[ $# -ge 2 ]] || die "Thiếu file zip"
            install_or_upgrade "$2" "${3:-}"
            ;;
        upgrade)
            [[ $# -ge 2 ]] || die "Thiếu file zip"
            install_or_upgrade "$2" "${3:-}"
            ;;
        uninstall)
            uninstall_all
            ;;
        status)
            show_status
            ;;
        ""|-h|--help|help)
            usage
            ;;
        *)
            die "Action không hợp lệ: ${action}"
            ;;
    esac
}

main "$@"