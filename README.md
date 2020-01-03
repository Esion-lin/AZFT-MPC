# MPC可信硬件
>Secure multi-party computation (MPC) enable a group to jointly perform a
computation without disclosing any participant’s private inputs. The participants agree on a function to compute, and then can use an MPC protocol
to jointly compute the output of that function on their secret inputs without
revealing them. 

MPC(Secure multi-party computation) 可以用于解决一组互不信任的参与方之间保护隐私的协同计算问题，SMPC 要确保输入的独立性，计算的正确性，同时不泄露各输入值给参与计算的其他成员。本项目实现了可信硬件下的安全多方计算通用框架。
#### 项目优势
**目前的MPC框架存在大量的问题**
- 计算效率低下。
- 无法保证可证明安全或安全性较低

尽管近年来研究者努力进行实用化技术的研究，并取得一些成果，但是离真正的大规模产业化应用还有一段距离。

**使用可信硬件的MPC**

将可信硬件用于安全多方计算是一种有效提高计算速度的方法，并且在安全性假设下，其相较于传统交互式的MPC具有更高的安全性优势。但目前主流的可信硬件大多采用SGX，但由于英特尔的信任源问题，无法保证数据安全。本系统开发了通用的可信硬件支持框架，可以嵌入任何可信硬件。

**与同类产品的比较**

本项目设计开发了基于可信软硬件的通用安全多方计算平台。符合以下特点：
* 异构性：该安全多方计算平台可以采用TPM、smartcard、TrustZone等各种实现，并非依赖于Intel SGX
* 高效性：该框架可支持全部通用安全多方计算任务，性能(通信量与运行时间)超过目前最先进的非可信软硬件的通用安全多方计算系统2~3个数量级。
* 强鲁棒性以及高安全性：在任意n-1计算方恶意攻击的情况下，仍然保证输出计算结果的正确性。该框架超过现有已和安全多方计算方案的安全水平。该框架可以做到mobile下的malicious安全。
	
*可证明安全下的异构*
<center> ![logical][logical]</center> 
<center>框架的逻辑结果和物理结构对比 </center> 
[logical]: ./image/logical.png "框架逻辑、物理结构"
不同的公司采用各自的底层可信硬件平台参与计算，信任源由单一的SGX到多信任源。在逻辑上可以规约到所有计算方直接与可信第三方交互。

*同类型安全性比较*

本框架与同类型相比在安全性上有较大优势，做到了mobile下的malicious安全，远远超过了目前市面上的主流框架。
<center>![tableofadvantage][tableofadvantage]</center> 
<center>与同类型框架安全性比较 </center> 
[tableofadvantage]: ./image/table.png "与同类型框架比较"

## 协议说明

协议流程分为秘钥交换， 运行指令交换，数据交换，以及进行多方计算四个流程。
- 秘钥交换
<br>协议需要两个非对称密钥对，以及一个对称秘钥对。在秘钥交换流程中，用户向网络广播其非对称密钥对：I.用户查询自己的可信硬件，可信硬件将在可信硬件中生成的公钥对中的公钥一起返回给用户，用户将该公钥分享到MPC网络上。同时用户监听MPC信道，收集所有收到的公钥对，并告知可信硬件。II.在I稳定后，可信硬件使用可信硬件中生成的签名私钥对所知的所有公钥进行签名。再由用户将该签名发送到MPC网络。同时检查收到的签名，检查正确后储存对应的公钥。
- 运行指令交换
<br> 运行指令交换目的是交换用户的运行协议以保证用户无法使用其他指令来进行作弊操作，向可信硬件发起签署请求，可信硬件对指令添加计数器，并使用生成的对称秘钥进行MAC，返回使用非对称秘钥对中用于加密的私钥对对称秘钥进行加密，并将该密文和MAC数据一起返回给用户。用户向MPC网络广播该密文。并监听MPC网络，储存接收的其他方发送的MAC数据。
- 数据交换
<br>与指令交换类似的，需要进行数据交换来确保用户运行的指令中的数据的正确性。可信硬件同样使用非对称秘钥对中用于加密的私钥对对称秘钥进行加密，然后使用对称秘钥对数据进行加密，最后使用该秘钥对数据和指令中使用的标签整合内容进行MAC计算。用户广播并监听。
<center> ![protocol0][protocol0]</center> 
<center>秘钥、数据交换过程 </center> 
[protocol0]: ./image/protocol_0.png "秘钥、数据等的交换"
- 计算
<br>用户需要进行计算时将标签、数据密文，指令，以及标签-数据密文的MAC、指令的MAC输入到可信硬件，可信硬件返回输出标签对应的密文结果和标签-密文MAC。



## 代码实现
#### 项目结构
- compiler
	- mixedProtocolsAnalysis
	- Protocols_Analysis
- sootOutput
- HW
- *.cpp
 	- main
 	-  network
 	-  protocol

>**HW** hardware 中是模拟可信硬件的类文件。

>**compiler** 负责将java code 编译成电路语句，输出到sootOutput文件夹，其中mixedProtocolsAnalysis是编译程序的编译运行文件，Protocols_Analysis是源文件。

>**protocol** 程序负责读入电路语句，对电路语句进行展开、去除数据依赖，与HW交互对指令进行MAC操作等。

>**network** 程序负责整个网络的通信，对数据进行序列化，提供缓冲区，储存交互的数据等。

>**main** 程序中运行了整个程序的业务流程，包括多次的数据交互，秘钥交换，指令的运行等。

>**其他文件** 包括了编译安装的脚本，一些中间函数等。

--------

#### 运行环境

系统环境：Linux

运行：
```shell
cd install_tool
#检查环境，或手动安装jsoncpp
bash ./install.sh
#编译java code 文件 bash ./compile.sh java_code项目路径  如
bash ./compile.sh ./compiler/Protocols_Analysis/src/programs/mexp
#运行程序
./main
```
#### Java code编译成电路语句
将java code 编译成电路语句，涉及到代码块的拼接，代码参数、宏定义的展开等。使用Protocols_Analysis程序对java文件进行编译得到电路程序，输出在sootOutput文件中。使用file_deal函数对该文件进行裁剪获取运行有效部分。
#### 电路语句展开实现
由于需要对指令进行数据有效性检查，需要添加MAC，在系统对指令进行签名时，需要对指令变量进行展开（针对数组变量等），此时要对程序段进行预运行，该预运行（展开）包括了
1. 对分支语句进行展开，以确保得到的指令可以添加计数器。
2. 对不合法指令进行检查，密文不能出现在分支语句中、密文不能出现在数组索引中等。 
3. 对数组元素进行改写，如arr[i11], 先确保i11是明文数据，不然返回报错，在用密文改写数组，如i11在运行到此指令时value为100，则得到arr_100。
4. 对代码进行预展开能确保程序是数据独立的，只有通过了预展开，才能证明代码段在输入密文数据前数据流已经确定了。

#### 协议运行实现
<center>![protocol1][protocol1]</center> 
<center>协议运行</center> 
[protocol1]: ./image/protocol_1.png "协议运行"
系统流程设计
1. 启动可信硬件程序 可信硬件程序启动后生成两对公钥对（一对用于签名，一对用于加密）。
2. 交换密钥 在该流程，I.用户查询自己的可信硬件，可信硬件将在（1）中生成的公钥对中的公钥一起返回给用户，用户将该公钥分享到MPC网络上。同时用户监听MPC信道，收集所有收到的公钥对，并告知可信硬件。II.在I稳定后，可信硬件使用（1）中生成的签名私钥对所知的所有公钥进行签名。再由用户将该签名发送到MPC网络。同时检查收到的签名，检查正确后储存对应的公钥。
3. 计算内容认证 已知多方用户需要多方计算的代码为ð，需要对ð进行解析，提取操作数和操作符，对数据相关的代码段进行优化，编译成电路语句，最后得到对应的指令流（由于需要确定指令有效，需要在可信硬件中对指令进行签名检查），此时所有用户得到了统一的指令流。
4. 数据交换 在该阶段，用户根据己方已知数据输入数据，将数据提交给可信硬件。此时，可信硬件生成本地的对称密钥，使用收集到的其他用户公钥对本地的对称密钥进行加密，再使用该对称密钥对数据进行加密，将混合的密文返回给用户，由用户分享到MPC网络。同时用户监听MPC信道收集所有收到的数据，将所有的数据整合后开始下一阶段。
5. 计算阶段 在该阶段，用户按指令流向可信硬件查询指令结果（使用对应的密文代替对应的操作数变量），可信硬件计算后向用户返回计算结果的密文。
6. 结果输出 可信硬件需要输出明文，需要所有的用户对输出明文进行确认。用户在指令流结束后向全网络广播其计算结束状态，所有用户确认其状态合法性使用可信硬件进行签名确认。用户收集所有的确认信息后将该签名提交给可信硬件，可信硬件返回最终结果的明文数据，用户得到最终结果后向MPC网络广播结果。

<center>![protocol2][protocol2]</center> 
<center>协议运行优化</center> 
[protocol2]: ./image/protocol_2.png "协议运行优化"