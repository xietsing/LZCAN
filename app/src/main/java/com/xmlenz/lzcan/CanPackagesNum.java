package com.xmlenz.lzcan;

public class CanPackagesNum {
    private String canName;
    private String packNumber1;
    private String packNumber2;
    private String packNumber3;

    public CanPackagesNum(String name ){
        canName = name;
        packNumber1 = "Can1:0";
        packNumber2 = "Can2:0";
        packNumber3 = "Can3:0";
    }


    public String getPackNumber1() {
        return packNumber1;
    }

    public void setPackNumber1(String packNumber1) {
        this.packNumber1 = "Can1 Packages Num:"+packNumber1;
    }

    public String getPackNumber2() {
        return packNumber2;
    }

    public void setPackNumber2(String packNumber2) {
        this.packNumber2 = "Can2 Packages Num:"+packNumber2;
    }

    public String getPackNumber3() {
        return packNumber3;
    }

    public void setPackNumber3(String packNumber3) {
        this.packNumber3 = "Can3 Packages Num:"+packNumber3;
    }

    public String getCanName() {
        return canName;
    }

    public void setCanName(String canName) {
        this.canName = canName;
    }
}
