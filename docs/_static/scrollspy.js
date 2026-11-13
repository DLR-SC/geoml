/* Highlight the section of the current page in the navigation sidebar while
 * scrolling ("scroll spy").
 *
 * The Read the Docs theme only highlights a section when its link is clicked.
 * This script applies the same "current" classes as the theme's own reset()
 * whenever the scroll position moves into another section.
 */
(function () {
    "use strict";

    // distance from the top of the window at which a section counts as reached
    var OFFSET = 90;

    function init() {
        var menu = document.querySelector(".wy-menu-vertical");
        if (!menu) {
            return;
        }

        // links to sections of this page, in document order
        var items = [];
        menu.querySelectorAll('a.reference.internal[href^="#"]').forEach(function (link) {
            var id = decodeURIComponent(link.getAttribute("href").slice(1));
            var target = id ? document.getElementById(id) : null;
            if (target) {
                items.push({ link: link, target: target });
            }
        });
        if (items.length === 0) {
            return;
        }
        // link of the page itself, active above its first section
        var pageLink = menu.querySelector('a.reference.internal[href="#"]');

        var active = null;

        function setCurrent(link) {
            menu.querySelectorAll(".current").forEach(function (el) {
                el.classList.remove("current");
                el.setAttribute("aria-expanded", "false");
            });
            link.classList.add("current");
            link.setAttribute("aria-expanded", "true");
            for (var el = link.parentElement; el && el !== menu; el = el.parentElement) {
                if (el.tagName === "LI" && /\btoctree-l\d+\b/.test(el.className)) {
                    el.classList.add("current");
                    el.setAttribute("aria-expanded", "true");
                    if (el.classList.contains("toctree-l1") && el.parentElement) {
                        el.parentElement.classList.add("current");
                    }
                }
            }
            keepVisible(link);
        }

        // scroll the sidebar (not the page) so that the highlighted link is visible
        function keepVisible(link) {
            var container = document.querySelector(".wy-side-scroll");
            if (!container) {
                return;
            }
            var header = document.querySelector(".wy-side-nav-search");
            var top = header ? header.getBoundingClientRect().bottom : 0;
            var rect = link.getBoundingClientRect();
            if (rect.top < top) {
                container.scrollTop -= top - rect.top + 10;
            } else if (rect.bottom > window.innerHeight) {
                container.scrollTop += rect.bottom - window.innerHeight + 10;
            }
        }

        function update() {
            var current = null;
            var atBottom = window.innerHeight + window.scrollY >=
                document.documentElement.scrollHeight - 2;
            if (atBottom) {
                current = items[items.length - 1];
            } else {
                for (var i = 0; i < items.length; i++) {
                    if (items[i].target.getBoundingClientRect().top - OFFSET <= 0) {
                        current = items[i];
                    }
                }
            }
            var link = current ? current.link : pageLink;
            if (link && link !== active) {
                active = link;
                setCurrent(link);
            }
        }

        // update at most every 50 ms while scrolling, and once after scrolling
        var scheduled = false;
        window.addEventListener("scroll", function () {
            if (!scheduled) {
                scheduled = true;
                window.setTimeout(function () {
                    scheduled = false;
                    update();
                }, 50);
            }
        }, { passive: true });

        // run after the theme has applied its own highlighting on page load
        window.setTimeout(update, 0);
    }

    if (document.readyState === "loading") {
        document.addEventListener("DOMContentLoaded", init);
    } else {
        init();
    }
})();
