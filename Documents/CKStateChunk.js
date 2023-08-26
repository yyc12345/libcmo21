
function FixTableStyle() {
    let tables = document.querySelectorAll("table");
    for (let index = 0; index < tables.length; index++) {
        let singletable = tables[index];
        singletable.setAttribute("border", "1");
        singletable.setAttribute("cellspacing", "1");
        singletable.setAttribute("cellpadding", "10");
    }
}

function TableCopyer() {
    let phs = document.querySelectorAll(".ph");
    for (let index = 0; index < phs.length; index++) {
        let ph = phs[index];
        let phparent = ph.parentNode;

        let phtarget = ph.getAttribute("target");
        if (phtarget === null) {
            phparent.removeChild(ph);
            continue;
        }

        let target = document.querySelector("#" + phtarget);
        if (target === null) {
            phparent.removeChild(ph);
            continue;
        }

        let clonetarget = target.cloneNode(true);
        clonetarget.removeAttribute("id");
        phparent.replaceChild(clonetarget, ph);
    }
}

window.onload=function() {
    FixTableStyle();
    TableCopyer();
};