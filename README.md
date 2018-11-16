<!--
#
# Copyright © 2018 Moxa Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
-->

# ALIYUN-IOT-SDK-C

> **Description:**
>
> Cloud: Create device instance for connecting by SDK program
>
> Host: Cross-compiling the SDK
>
> Target: Executing the SDK program

## Cloud (Aliyun)

### Sign in to Cloud

* Sign in to [Aliyun IoT Cloud][cloud]. If you do not have an account, please register a new one.

### Create Product

1. In the left navigation pane, expand **Devices** and then choose **Product**.

    ![create_product_01][create_product_01]

2. On the **Products** page, choose **Create Product** and fill in the necessary information then choose **OK**.

    ![create_product_02][create_product_02]

3. Finish creating the Product.

    ![create_product_03][create_product_03]

### Create Device

1. In the left navigation pane, expand **Devices** and then choose **Device**.

    ![create_device_01][create_device_01]

2. On the **Devices** page, choose **Add Device** and fill in the necessary information then choose **OK**.

    ![create_device_02][create_device_02]

3. Finish creating the Device.

    ![create_device_03][create_device_03]

### Create Topic

1. In the left navigation pane, expand **Devices** and then choose **Product**. On the **Products** page, search for the **[Product you create before]** then choose **View**.

    ![create_topic_01][create_topic_01]

2. On the **Product Details** page, choose **Notifications** > **Create Topic Category**.

    ![create_topic_02][create_topic_02]

3. On the **Create Topic Category** page, fill in the necessary information then choose **OK**.

    ![create_topic_03][create_topic_03]

4. Finish creating the Topic.

    ![create_topic_04][create_topic_04]

### Copy Device Identification

1. In the left navigation pane, expand **Devices** and then choose **Device**. On the **Devices** page, search for the **[Device you create before]** then choose **View**.

    ![copy_device_identification_01][copy_device_identification_01]

2. On the **Device Information** tab, you can find and copy the device identification **ProductKey**, **DeviceName** and **DeviceSecret**.

    ![copy_device_identification_02][copy_device_identification_02]

### View Device Log

1. In the left navigation pane, expand **Devices** and then choose **Product**. On the **Products** page, search for the **[Product you create before]** then choose **View**.

    ![view_device_log_01][view_device_log_01]

2. On the **Device Log** tab, you can view all device log in subtabs of **Device Actitivity Analysis**, **Upstream Analysis** and **Downstream Analysis**.

    ![view_device_log_02][view_device_log_02]
    ![view_device_log_03][view_device_log_03]
    ![view_device_log_04][view_device_log_04]

## Host (x86_64-linux)

### Setup the Environment

1. Setup a network connection to allow host able to access the network.

2. Install GNU cross-toolchain provide by MOXA.

3. Install following package from package manager.

    ```
    cmake git rsync
    ```

### Build the SDK

1. Setup dependencies and SDK to output directory.

    ```
    $ ./setup.sh
    ```
    * For more setup.sh options.

    ```
    $ ./setup.sh --help

    Usage: ./setup.sh [options]

    Options:
        -git                Git repository of SDK.
                            Default: https://github.com/aliyun/iotkit-embedded.git

        -ver                Version of SDK.
                            Default: RELEASED_V2.03

        --toolchain         GNU cross-toolchain directory.
                            Default: /usr/local/bin/gcc-linaro-5.1-2015.08-x86_64_arm-linux-gnueabihf

        --help              Display this help and exit.

    Examples:
        Default             ./setup.sh
        Specify             ./setup.sh -git https://github.com/aliyun/iotkit-embedded.git -ver RELEASED_V2.03
                            ./setup.sh --toolchain /usr/local/bin/gcc-linaro-5.1-2015.08-x86_64_arm-linux-gnueabihf
    ```

2. Add the device identification **ProductKey**, **DeviceName** and **DeviceSecret** to SDK sample code such as example **mqtt-example.c**. [[Copy Device Identification](#copy-device-identification)]

    ```
    $ vim output/sdk_aliyun/sample/mqtt/mqtt-example.c
    ```
    ```
    #define PRODUCT_KEY             "a14qsGgMTtk"
    #define DEVICE_NAME             "Example_Device"
    #define DEVICE_SECRET           "CTA83oLWKq2n0YgvveFZusvVqeA0EVTq"
    ```

3. Build the whole SDK.

    ```
    $ ./build.sh
    ```
    * All compiled SDK program can be found in the following directory, including example **mqtt-example**.

    ```
    $ tree output/sdk_aliyun/output/release/bin
    output/sdk_aliyun/output/release/bin
    ├── coap-example
    ├── http-example
    ├── mqtt-example
    ├── mqtt_multi_thread-example
    ├── mqtt_rrpc-example
    ├── ota_mqtt-example
    ├── sdk-testsuites
    └── shadow-example
    ```

* Note

    ```
    In general, the setup.sh only needs to be executed once.
    The build.sh needs to be executed after any code change of the SDK.
    ```

## Target (arm-linux)

### Setup the Environment

1. Setup a network connection to allow target able to access the network.

2. Copy compiled SDK program from host to target

    ```
    $ tree
    .
    └── mqtt-example
    ```

### Execute the SDK

1. Execute SDK program that cross-compiled by host.

    ```
    $ ./mqtt-example
    ```
    * You need to install the dependency library for the SDK program if any not found.

2. [View device log on cloud](#view-device-log).

## Reference

[1] [https://github.com/aliyun/iotkit-embedded][Reference_01]

[2] [https://www.alibabacloud.com/help/product/30520.htm][Reference_02]

[comment]: # (Images)
[create_product_01]: readme/create_product_01.png
[create_product_02]: readme/create_product_02.png
[create_product_03]: readme/create_product_03.png

[create_device_01]: readme/create_device_01.png
[create_device_02]: readme/create_device_02.png
[create_device_03]: readme/create_device_03.png

[create_topic_01]: readme/create_topic_01.png
[create_topic_02]: readme/create_topic_02.png
[create_topic_03]: readme/create_topic_03.png
[create_topic_04]: readme/create_topic_04.png

[copy_device_identification_01]: readme/copy_device_identification_01.png
[copy_device_identification_02]: readme/copy_device_identification_02.png

[view_device_log_01]: readme/view_device_log_01.png
[view_device_log_02]: readme/view_device_log_02.png
[view_device_log_03]: readme/view_device_log_03.png
[view_device_log_04]: readme/view_device_log_04.png

[comment]: # (Links)
[cloud]: https://iot.console.aliyun.com
[Reference_01]: https://github.com/aliyun/iotkit-embedded
[Reference_02]: https://www.alibabacloud.com/help/product/30520.htm

